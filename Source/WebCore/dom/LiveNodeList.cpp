/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2006, 2007, 2008, 2010, 2013 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "LiveNodeList.h"

#include "ClassNodeList.h"
#include "Element.h"
#include "ElementTraversal.h"
#include "HTMLCollection.h"
#include "TagNodeList.h"

namespace WebCore {

ContainerNode& LiveNodeList::rootNode() const
{
    if (isRootedAtDocument() && ownerNode().inDocument())
        return ownerNode().document();

    return ownerNode();
}

template <class NodeListType>
inline bool isMatchingElement(const NodeListType*, Element*);

template <> inline bool isMatchingElement(const LiveNodeList* nodeList, Element* element)
{
    return nodeList->nodeMatches(element);
}

template <> inline bool isMatchingElement(const HTMLTagNodeList* nodeList, Element* element)
{
    return nodeList->nodeMatchesInlined(element);
}

template <> inline bool isMatchingElement(const ClassNodeList* nodeList, Element* element)
{
    return nodeList->nodeMatchesInlined(element);
}

template <class NodeListType>
inline Element* firstMatchingElement(const NodeListType* nodeList, ContainerNode& root)
{
    Element* element = ElementTraversal::firstWithin(&root);
    while (element && !isMatchingElement(nodeList, element))
        element = ElementTraversal::next(element, &root);
    return element;
}

template <class NodeListType>
inline Element* lastMatchingElement(const NodeListType* nodeList, ContainerNode& root)
{
    Element* element = ElementTraversal::lastWithin(&root);
    while (element && !isMatchingElement(nodeList, element))
        element = ElementTraversal::previous(element, &root);
    return element;
}

template <class NodeListType>
inline Element* nextMatchingElement(const NodeListType* nodeList, Element* current, ContainerNode& root)
{
    do {
        current = ElementTraversal::next(current, &root);
    } while (current && !isMatchingElement(nodeList, current));
    return current;
}

template <class NodeListType>
inline Element* previousMatchingElement(const NodeListType* nodeList, Element* current, ContainerNode& root)
{
    do {
        current = ElementTraversal::previous(current, &root);
    } while (current && !isMatchingElement(nodeList, current));
    return current;
}

template <class NodeListType>
inline Element* traverseMatchingElementsForward(const NodeListType* nodeList, Element& current, unsigned count, unsigned& traversedCount, ContainerNode& root)
{
    Element* element = &current;
    for (traversedCount = 0; traversedCount < count; ++traversedCount) {
        element = nextMatchingElement(nodeList, element, root);
        if (!element)
            return nullptr;
    }
    return element;
}

template <class NodeListType>
inline Element* traverseMatchingElementsBackward(const NodeListType* nodeList, Element& current, unsigned count, ContainerNode& root)
{
    Element* element = &current;
    for (; count; --count) {
        element = previousMatchingElement(nodeList, element, root);
        if (!element)
            return nullptr;
    }
    return element;
}

Element* LiveNodeList::collectionFirst() const
{
    auto& root = rootNode();
    if (type() == Type::HTMLTagNodeListType)
        return firstMatchingElement(static_cast<const HTMLTagNodeList*>(this), root);
    if (type() == Type::ClassNodeListType)
        return firstMatchingElement(static_cast<const ClassNodeList*>(this), root);
    return firstMatchingElement(static_cast<const LiveNodeList*>(this), root);
}

Element* LiveNodeList::collectionLast() const
{
    auto& root = rootNode();
    if (type() == Type::HTMLTagNodeListType)
        return lastMatchingElement(static_cast<const HTMLTagNodeList*>(this), root);
    if (type() == Type::ClassNodeListType)
        return lastMatchingElement(static_cast<const ClassNodeList*>(this), root);
    return lastMatchingElement(static_cast<const LiveNodeList*>(this), root);
}

Element* LiveNodeList::collectionTraverseForward(Element& current, unsigned count, unsigned& traversedCount) const
{
    auto& root = rootNode();
    if (type() == Type::HTMLTagNodeListType)
        return traverseMatchingElementsForward(static_cast<const HTMLTagNodeList*>(this), current, count, traversedCount, root);
    if (type() == Type::ClassNodeListType)
        return traverseMatchingElementsForward(static_cast<const ClassNodeList*>(this), current, count, traversedCount, root);
    return traverseMatchingElementsForward(static_cast<const LiveNodeList*>(this), current, count, traversedCount, root);
}

Element* LiveNodeList::collectionTraverseBackward(Element& current, unsigned count) const
{
    auto& root = rootNode();
    if (type() == Type::HTMLTagNodeListType)
        return traverseMatchingElementsBackward(static_cast<const HTMLTagNodeList*>(this), current, count, root);
    if (type() == Type::ClassNodeListType)
        return traverseMatchingElementsBackward(static_cast<const ClassNodeList*>(this), current, count, root);
    return traverseMatchingElementsBackward(static_cast<const LiveNodeList*>(this), current, count, root);
}

unsigned LiveNodeList::length() const
{
    return m_indexCache.nodeCount(*this);
}

Node* LiveNodeList::item(unsigned offset) const
{
    return m_indexCache.nodeAt(*this, offset);
}

size_t LiveNodeList::memoryCost() const
{
    return m_indexCache.memoryCost();
}

void LiveNodeList::invalidateCache(Document& document) const
{
    if (!m_indexCache.hasValidCache())
        return;
    document.unregisterNodeList(const_cast<LiveNodeList&>(*this));
    m_indexCache.invalidate();
}

Node* LiveNodeList::namedItem(const AtomicString& elementId) const
{
    // FIXME: Why doesn't this look into the name attribute like HTMLCollection::namedItem does?
    Node& rootNode = this->rootNode();

    if (rootNode.inDocument()) {
        Element* element = rootNode.treeScope().getElementById(elementId);
        if (element && nodeMatches(element) && element->isDescendantOf(&rootNode))
            return element;
        if (!element)
            return 0;
        // In the case of multiple nodes with the same name, just fall through.
    }

    unsigned length = this->length();
    for (unsigned i = 0; i < length; i++) {
        Node* node = item(i);
        if (!node->isElementNode())
            continue;
        Element* element = toElement(node);
        // FIXME: This should probably be using getIdAttribute instead of idForStyleResolution.
        if (element->hasID() && element->idForStyleResolution() == elementId)
            return node;
    }

    return 0;
}

} // namespace WebCore
