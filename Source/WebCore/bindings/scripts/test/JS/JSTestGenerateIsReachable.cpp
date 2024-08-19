/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "JSTestGenerateIsReachable.h"

#include "TestGenerateIsReachable.h"
#include <wtf/GetPtr.h>

using namespace JSC;

namespace WebCore {

/* Hash table for constructor */

static const struct CompactHashIndex JSTestGenerateIsReachableConstructorTableIndex[1] = {
    { -1, -1 },
};


static const HashTableValue JSTestGenerateIsReachableConstructorTableValues[0] =
{
};

static const HashTable JSTestGenerateIsReachableConstructorTable = { 0, 0, false, JSTestGenerateIsReachableConstructorTableValues, 0, JSTestGenerateIsReachableConstructorTableIndex };
const ClassInfo JSTestGenerateIsReachableConstructor::s_info = { "TestGenerateIsReachableConstructor", &Base::s_info, &JSTestGenerateIsReachableConstructorTable, 0, CREATE_METHOD_TABLE(JSTestGenerateIsReachableConstructor) };

JSTestGenerateIsReachableConstructor::JSTestGenerateIsReachableConstructor(Structure* structure, JSDOMGlobalObject* globalObject)
    : DOMConstructorObject(structure, globalObject)
{
}

void JSTestGenerateIsReachableConstructor::finishCreation(VM& vm, JSDOMGlobalObject* globalObject)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
    putDirect(vm, vm.propertyNames->prototype, JSTestGenerateIsReachablePrototype::self(vm, globalObject), DontDelete | ReadOnly);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), ReadOnly | DontDelete | DontEnum);
}

bool JSTestGenerateIsReachableConstructor::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSTestGenerateIsReachableConstructor, JSDOMWrapper>(exec, JSTestGenerateIsReachableConstructorTable, jsCast<JSTestGenerateIsReachableConstructor*>(object), propertyName, slot);
}

/* Hash table for prototype */

static const struct CompactHashIndex JSTestGenerateIsReachablePrototypeTableIndex[2] = {
    { -1, -1 },
    { 0, -1 },
};


static const HashTableValue JSTestGenerateIsReachablePrototypeTableValues[1] =
{
    { "constructor", DontEnum | ReadOnly, NoIntrinsic, (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestGenerateIsReachableConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) },
};

static const HashTable JSTestGenerateIsReachablePrototypeTable = { 1, 1, true, JSTestGenerateIsReachablePrototypeTableValues, 0, JSTestGenerateIsReachablePrototypeTableIndex };
const ClassInfo JSTestGenerateIsReachablePrototype::s_info = { "TestGenerateIsReachablePrototype", &Base::s_info, &JSTestGenerateIsReachablePrototypeTable, 0, CREATE_METHOD_TABLE(JSTestGenerateIsReachablePrototype) };

JSObject* JSTestGenerateIsReachablePrototype::self(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMPrototype<JSTestGenerateIsReachable>(vm, globalObject);
}

bool JSTestGenerateIsReachablePrototype::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSTestGenerateIsReachablePrototype* thisObject = jsCast<JSTestGenerateIsReachablePrototype*>(object);
    return getStaticPropertySlot<JSTestGenerateIsReachablePrototype, JSObject>(exec, JSTestGenerateIsReachablePrototypeTable, thisObject, propertyName, slot);
}

const ClassInfo JSTestGenerateIsReachable::s_info = { "TestGenerateIsReachable", &Base::s_info, 0, 0 , CREATE_METHOD_TABLE(JSTestGenerateIsReachable) };

JSTestGenerateIsReachable::JSTestGenerateIsReachable(Structure* structure, JSDOMGlobalObject* globalObject, PassRefPtr<TestGenerateIsReachable> impl)
    : JSDOMWrapper(structure, globalObject)
    , m_impl(impl.leakRef())
{
}

void JSTestGenerateIsReachable::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));
}

JSObject* JSTestGenerateIsReachable::createPrototype(VM& vm, JSGlobalObject* globalObject)
{
    return JSTestGenerateIsReachablePrototype::create(vm, globalObject, JSTestGenerateIsReachablePrototype::createStructure(vm, globalObject, globalObject->objectPrototype()));
}

void JSTestGenerateIsReachable::destroy(JSC::JSCell* cell)
{
    JSTestGenerateIsReachable* thisObject = static_cast<JSTestGenerateIsReachable*>(cell);
    thisObject->JSTestGenerateIsReachable::~JSTestGenerateIsReachable();
}

JSTestGenerateIsReachable::~JSTestGenerateIsReachable()
{
    releaseImplIfNotNull();
}

bool JSTestGenerateIsReachable::getOwnPropertySlot(JSObject* object, ExecState* exec, PropertyName propertyName, PropertySlot& slot)
{
    JSTestGenerateIsReachable* thisObject = jsCast<JSTestGenerateIsReachable*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    return Base::getOwnPropertySlot(thisObject, exec, propertyName, slot);
}

EncodedJSValue jsTestGenerateIsReachableConstructor(ExecState* exec, JSObject* baseValue, EncodedJSValue thisValue, PropertyName)
{
    UNUSED_PARAM(baseValue);
    UNUSED_PARAM(thisValue);
    JSTestGenerateIsReachablePrototype* domObject = jsDynamicCast<JSTestGenerateIsReachablePrototype*>(baseValue);
    if (!domObject)
        return throwVMTypeError(exec);
    return JSValue::encode(JSTestGenerateIsReachable::getConstructor(exec->vm(), domObject->globalObject()));
}

JSValue JSTestGenerateIsReachable::getConstructor(VM& vm, JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestGenerateIsReachableConstructor>(vm, jsCast<JSDOMGlobalObject*>(globalObject));
}

static inline bool isObservable(JSTestGenerateIsReachable* jsTestGenerateIsReachable)
{
    if (jsTestGenerateIsReachable->hasCustomProperties())
        return true;
    return false;
}

bool JSTestGenerateIsReachableOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor)
{
    JSTestGenerateIsReachable* jsTestGenerateIsReachable = jsCast<JSTestGenerateIsReachable*>(handle.get().asCell());
    if (!isObservable(jsTestGenerateIsReachable))
        return false;
    TestGenerateIsReachable* root = &jsTestGenerateIsReachable->impl();
    return visitor.containsOpaqueRoot(root);
}

void JSTestGenerateIsReachableOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    JSTestGenerateIsReachable* jsTestGenerateIsReachable = jsCast<JSTestGenerateIsReachable*>(handle.get().asCell());
    DOMWrapperWorld& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestGenerateIsReachable->impl(), jsTestGenerateIsReachable);
    jsTestGenerateIsReachable->releaseImpl();
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestGenerateIsReachable@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore23TestGenerateIsReachableE[]; }
#endif
#endif
JSC::JSValue toJS(JSC::ExecState* exec, JSDOMGlobalObject* globalObject, TestGenerateIsReachable* impl)
{
    if (!impl)
        return jsNull();
    if (JSValue result = getExistingWrapper<JSTestGenerateIsReachable>(exec, impl))
        return result;

#if ENABLE(BINDING_INTEGRITY)
    void* actualVTablePointer = *(reinterpret_cast<void**>(impl));
#if PLATFORM(WIN)
    void* expectedVTablePointer = reinterpret_cast<void*>(__identifier("??_7TestGenerateIsReachable@WebCore@@6B@"));
#else
    void* expectedVTablePointer = &_ZTVN7WebCore23TestGenerateIsReachableE[2];
#if COMPILER(CLANG)
    // If this fails TestGenerateIsReachable does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    COMPILE_ASSERT(__is_polymorphic(TestGenerateIsReachable), TestGenerateIsReachable_is_not_polymorphic);
#endif
#endif
    // If you hit this assertion you either have a use after free bug, or
    // TestGenerateIsReachable has subclasses. If TestGenerateIsReachable has subclasses that get passed
    // to toJS() we currently require TestGenerateIsReachable you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    return createNewWrapper<JSTestGenerateIsReachable>(exec, globalObject, impl);
}

TestGenerateIsReachable* toTestGenerateIsReachable(JSC::JSValue value)
{
    return value.inherits(JSTestGenerateIsReachable::info()) ? &jsCast<JSTestGenerateIsReachable*>(value)->impl() : 0;
}

}
