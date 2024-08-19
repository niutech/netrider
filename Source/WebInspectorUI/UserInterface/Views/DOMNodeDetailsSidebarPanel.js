/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

WebInspector.DOMNodeDetailsSidebarPanel = function() {
    WebInspector.DOMDetailsSidebarPanel.call(this, "dom-node-details", WebInspector.UIString("Node"), WebInspector.UIString("Node"), "Images/NavigationItemAngleBrackets.svg", "2");

    WebInspector.domTreeManager.addEventListener(WebInspector.DOMTreeManager.Event.AttributeModified, this._attributesChanged, this);
    WebInspector.domTreeManager.addEventListener(WebInspector.DOMTreeManager.Event.AttributeRemoved, this._attributesChanged, this);

    this.element.classList.add(WebInspector.DOMNodeDetailsSidebarPanel.StyleClassName);

    this._identityNodeTypeRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Type"));
    this._identityNodeNameRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Name"));
    this._identityNodeValueRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Value"));

    var identityGroup = new WebInspector.DetailsSectionGroup([this._identityNodeTypeRow, this._identityNodeNameRow, this._identityNodeValueRow]);
    var identitySection = new WebInspector.DetailsSection("dom-node-identity", WebInspector.UIString("Identity"), [identityGroup]);

    this._attributesDataGridRow = new WebInspector.DetailsSectionDataGridRow(null, WebInspector.UIString("No Attributes"));

    var attributesGroup = new WebInspector.DetailsSectionGroup([this._attributesDataGridRow]);
    var attributesSection = new WebInspector.DetailsSection("dom-node-attributes", WebInspector.UIString("Attributes"), [attributesGroup]);

    this._propertiesRow = new WebInspector.DetailsSectionRow;

    var propertiesGroup = new WebInspector.DetailsSectionGroup([this._propertiesRow]);
    var propertiesSection = new WebInspector.DetailsSection("dom-node-properties", WebInspector.UIString("Properties"), [propertiesGroup]);

    this._eventListenersSectionGroup = new WebInspector.DetailsSectionGroup;
    var eventListenersSection = new WebInspector.DetailsSection("dom-node-event-listeners", WebInspector.UIString("Event Listeners"), [this._eventListenersSectionGroup]);    

    this._accessibilityEmptyRow = new WebInspector.DetailsSectionRow(WebInspector.UIString("No Accessibility Information"));
    this._accessibilityNodeCheckedRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Checked"));
    this._accessibilityNodeDisabledRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Disabled"));
    this._accessibilityNodeExpandedRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Expanded"));
    this._accessibilityNodeIgnoredRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Ignored"));
    this._accessibilityNodeInvalidRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Invalid"));
    this._accessibilityNodeLabelRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Label"));
    this._accessibilityNodePressedRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Pressed"));
    this._accessibilityNodeReadonlyRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Readonly"));
    this._accessibilityNodeRequiredRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Required"));
    this._accessibilityNodeRoleRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Role"));
    this._accessibilityNodeSelectedRow = new WebInspector.DetailsSectionSimpleRow(WebInspector.UIString("Selected"));
    
    this._accessibilityGroup = new WebInspector.DetailsSectionGroup([this._accessibilityEmptyRow]);
    var accessibilitySection = new WebInspector.DetailsSection("dom-node-accessibility", WebInspector.UIString("Accessibility"), [this._accessibilityGroup]);    
    
    this.element.appendChild(identitySection.element);
    this.element.appendChild(attributesSection.element);
    this.element.appendChild(propertiesSection.element);
    this.element.appendChild(eventListenersSection.element);
    this.element.appendChild(accessibilitySection.element);
};

WebInspector.DOMNodeDetailsSidebarPanel.StyleClassName = "dom-node";
WebInspector.DOMNodeDetailsSidebarPanel.PropertiesObjectGroupName = "dom-node-details-sidebar-properties-object-group";

WebInspector.DOMNodeDetailsSidebarPanel.prototype = {
    constructor: WebInspector.DOMNodeDetailsSidebarPanel,

    // Public

    refresh: function()
    {
        var domNode = this.domNode;
        if (!domNode)
            return;

        this._identityNodeTypeRow.value = this._nodeTypeDisplayName();
        this._identityNodeNameRow.value = domNode.nodeNameInCorrectCase();
        this._identityNodeValueRow.value = domNode.nodeValue();

        this._refreshAttributes();
        this._refreshProperties();
        this._refreshEventListeners();
        this._refreshAccessibility();
    },

    // Private

    _refreshAttributes: function()
    {
        this._attributesDataGridRow.dataGrid = this._createAttributesDataGrid();
    },

    _refreshProperties: function()
    {
        var domNode = this.domNode;
        if (!domNode)
            return;

        RuntimeAgent.releaseObjectGroup(WebInspector.DOMNodeDetailsSidebarPanel.PropertiesObjectGroupName);
        WebInspector.RemoteObject.resolveNode(domNode, WebInspector.DOMNodeDetailsSidebarPanel.PropertiesObjectGroupName, nodeResolved.bind(this));

        function nodeResolved(object)
        {
            if (!object)
                return;

            // Bail if the DOM node changed while we were waiting for the async response.
            if (this.domNode !== domNode)
                return;

            function collectPrototypes()
            {
                // This builds an object with numeric properties. This is easier than dealing with arrays
                // with the way RemoteObject works. Start at 1 since we use parseInt later and parseInt
                // returns 0 for non-numeric strings make it ambiguous.
                var prototype = this;
                var result = [];
                var counter = 1;

                while (prototype) {
                    result[counter++] = prototype;
                    prototype = prototype.__proto__;
                }

                return result;
            }

            object.callFunction(collectPrototypes, undefined, nodePrototypesReady.bind(this));
            object.release();
        }

        function nodePrototypesReady(object)
        {
            if (!object)
                return;

            // Bail if the DOM node changed while we were waiting for the async response.
            if (this.domNode !== domNode)
                return;

            object.getOwnProperties(fillSection.bind(this));
        }

        function fillSection(prototypes)
        {
            if (!prototypes)
                return;

            // Bail if the DOM node changed while we were waiting for the async response.
            if (this.domNode !== domNode)
                return;

            var element = this._propertiesRow.element;
            element.removeChildren();

            // Get array of prototype user-friendly names.
            for (var i = 0; i < prototypes.length; ++i) {
                // The only values we care about are numeric, as assigned in collectPrototypes.
                if (!parseInt(prototypes[i].name, 10))
                    continue;

                var prototype = prototypes[i].value;
                var title = prototype.description;
                if (title.match(/Prototype$/))
                    title = title.replace(/Prototype$/, WebInspector.UIString(" (Prototype)"));
                else if (title === "Object")
                    title = title + WebInspector.UIString(" (Prototype)");

                var propertiesSection = new WebInspector.ObjectPropertiesSection(prototype);

                var detailsSection = new WebInspector.DetailsSection(prototype.description.hash + "-prototype-properties", title, null, null, true);
                detailsSection.groups[0].rows = [new WebInspector.DetailsSectionPropertiesRow(propertiesSection)];

                element.appendChild(detailsSection.element);
            }
        }
    },

    _refreshEventListeners: function()
    {
        var domNode = this.domNode;
        if (!domNode)
            return;

        domNode.eventListeners(eventListenersCallback.bind(this));

        function eventListenersCallback(error, eventListeners)
        {
            if (error)
                return;

            // Bail if the DOM node changed while we were waiting for the async response.
            if (this.domNode !== domNode)
                return;

            var eventListenerTypes = [];
            var eventListenerSections = {};
            for (var i = 0; i < eventListeners.length; ++i) {
                var eventListener = eventListeners[i];
                eventListener.node = WebInspector.domTreeManager.nodeForId(eventListener.nodeId);

                var type = eventListener.type;
                var section = eventListenerSections[type];
                if (!section) {
                    section = new WebInspector.EventListenerSection(type, domNode.id);
                    eventListenerSections[type] = section;
                    eventListenerTypes.push(type);
                }

                section.addListener(eventListener);
            }

            if (!eventListenerTypes.length) {
                var emptyRow = new WebInspector.DetailsSectionRow(WebInspector.UIString("No Event Listeners"));
                emptyRow.showEmptyMessage();
                this._eventListenersSectionGroup.rows = [emptyRow];
                return;
            }

            eventListenerTypes.sort();

            var rows = [];
            for (var i = 0; i < eventListenerTypes.length; ++i)
                rows.push(eventListenerSections[eventListenerTypes[i]]);
            this._eventListenersSectionGroup.rows = rows;
        }
    },

    _refreshAccessibility: (function(){

        var properties = {};
        var domNode;

        function booleanValueToLocalizedStringIfTrue(property) {
            if (properties[property])
                return WebInspector.UIString("Yes");
            return "";
        }

        function booleanValueToLocalizedStringIfPropertyDefined(property) {
            if (properties[property] !== undefined) {
                if (properties[property])
                    return WebInspector.UIString("Yes");
                else
                    return WebInspector.UIString("No");
            }
            return "";
        }

        function accessibilityPropertiesCallback(accessibilityProperties) {

            if (this.domNode !== domNode)
                return;

            // Make sure the current set of properties is available in the closure scope for the helper functions.
            properties = accessibilityProperties;

            if (accessibilityProperties && accessibilityProperties.exists) {

                var checked = "";
                if (accessibilityProperties.checked !== undefined) {
                    if (accessibilityProperties.checked === DOMAgent.AccessibilityPropertiesChecked.True)
                        checked = WebInspector.UIString("Yes");
                    else if (accessibilityProperties.checked === DOMAgent.AccessibilityPropertiesChecked.Mixed)
                        checked = WebInspector.UIString("Mixed");
                    else // DOMAgent.AccessibilityPropertiesChecked.False
                        checked = WebInspector.UIString("No");
                }

                var disabled = booleanValueToLocalizedStringIfTrue("disabled");
                var expanded = booleanValueToLocalizedStringIfPropertyDefined("expanded");
                
                var ignored = "";
                if (accessibilityProperties.ignored) {
                    ignored = WebInspector.UIString("Yes");
                    if (accessibilityProperties.hidden)
                        ignored = WebInspector.UIString("%s (hidden)").format(ignored);
                    else if (accessibilityProperties.ignoredByDefault)
                        ignored = WebInspector.UIString("%s (default)").format(ignored);
                }

                var invalid = accessibilityProperties.invalid ? accessibilityProperties.invalid : "";

                // FIXME: label will always come back as empty. Blocked by http://webkit.org/b/121134
                var label = accessibilityProperties.label;
                if (label && label !== domNode.getAttribute("aria-label"))
                    label = WebInspector.UIString("%s (computed)").format(label);

                var pressed = booleanValueToLocalizedStringIfPropertyDefined("pressed");
                var readonly = booleanValueToLocalizedStringIfTrue("readonly");
                var required = booleanValueToLocalizedStringIfPropertyDefined("required");

                var role = accessibilityProperties.role;
                if (role === "" || role === "unknown")
                    role = WebInspector.UIString("No exact ARIA role match.");
                else if (role) {
                    if (!domNode.getAttribute("role"))
                        role = WebInspector.UIString("%s (default)").format(role);
                    else if (domNode.getAttribute("role") !== role)
                        role = WebInspector.UIString("%s (computed)").format(role);
                }

                var selected = booleanValueToLocalizedStringIfTrue("selected");

                // Assign all the properties to their respective views.
                this._accessibilityNodeCheckedRow.value = checked;
                this._accessibilityNodeDisabledRow.value = disabled;
                this._accessibilityNodeExpandedRow.value = expanded;
                this._accessibilityNodeIgnoredRow.value = ignored;
                this._accessibilityNodeInvalidRow.value = invalid;
                this._accessibilityNodeLabelRow.value = label;
                this._accessibilityNodePressedRow.value = pressed;
                this._accessibilityNodeReadonlyRow.value = readonly;
                this._accessibilityNodeRequiredRow.value = required;
                this._accessibilityNodeRoleRow.value = role;
                this._accessibilityNodeSelectedRow.value = selected;

                // Display order, not alphabetical as above.
                this._accessibilityGroup.rows = [

                    // Global properties for all elements.
                    this._accessibilityNodeIgnoredRow,
                    this._accessibilityNodeRoleRow,
                    this._accessibilityNodeLabelRow,

                    // Properties exposed for all input-type elements.
                    this._accessibilityNodeDisabledRow,
                    this._accessibilityNodeInvalidRow,
                    this._accessibilityNodeRequiredRow,

                    // Role-specific properties.
                    this._accessibilityNodeCheckedRow,
                    this._accessibilityNodeExpandedRow,
                    this._accessibilityNodePressedRow,
                    this._accessibilityNodeReadonlyRow,
                    this._accessibilityNodeSelectedRow

                ];

                this._accessibilityEmptyRow.hideEmptyMessage();

            } else {
                this._accessibilityGroup.rows = [this._accessibilityEmptyRow];
                this._accessibilityEmptyRow.showEmptyMessage();
            }
        }

        function refreshAX() {
            // Make sure the domNode is available in the closure scope.
            domNode = this.domNode;
            if (!domNode)
                return;
            domNode.accessibilityProperties(accessibilityPropertiesCallback.bind(this));
        }

        return refreshAX;
    }()),

    _attributesChanged: function(event)
    {
        if (event.data.node !== this.domNode)
            return;
        this._refreshAttributes();
        this._refreshAccessibility();
    },

    _nodeTypeDisplayName: function()
    {
        switch (this.domNode.nodeType()) {
        case Node.ELEMENT_NODE:
            return WebInspector.UIString("Element");
        case Node.TEXT_NODE:
            return WebInspector.UIString("Text Node");
        case Node.COMMENT_NODE:
            return WebInspector.UIString("Comment");
        case Node.DOCUMENT_NODE:
            return WebInspector.UIString("Document");
        case Node.DOCUMENT_TYPE_NODE:
            return WebInspector.UIString("Document Type");
        case Node.DOCUMENT_FRAGMENT_NODE:
            return WebInspector.UIString("Document Fragment");
        case Node.CDATA_SECTION_NODE:
            return WebInspector.UIString("Character Data");
        case Node.PROCESSING_INSTRUCTION_NODE:
            return WebInspector.UIString("Processing Instruction");
        default:
            console.error("Unknown DOM node type: ", this.domNode.nodeType());
            return this.domNode.nodeType();
        }
    },

    _createAttributesDataGrid: function()
    {
        var domNode = this.domNode;
        if (!domNode || !domNode.hasAttributes())
            return null;

        var columns = {name: {title: WebInspector.UIString("Name"), width: "30%"}, value: {title: WebInspector.UIString("Value")}};
        var dataGrid = new WebInspector.DataGrid(columns);

        var attributes = domNode.attributes();
        for (var i = 0; i < attributes.length; ++i) {
            var attribute = attributes[i];

            var node = new WebInspector.DataGridNode({name: attribute.name, value: attribute.value || ""}, false);
            node.selectable = true;

            dataGrid.appendChild(node);
        }

        return dataGrid;
    }
};

WebInspector.DOMNodeDetailsSidebarPanel.prototype.__proto__ = WebInspector.DOMDetailsSidebarPanel.prototype;
