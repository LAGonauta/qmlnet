#include <QmlNet/qml/JsNetArray.h>
#include <QmlNet/types/NetMethodInfo.h>
#include <QmlNet/types/Callbacks.h>
#include <QmlNet/types/NetTypeArrayFacade.h>
#include <QVariant>
#include <private/qv4qobjectwrapper_p.h>

QT_BEGIN_NAMESPACE

using namespace QV4;

DEFINE_OBJECT_VTABLE(NetArray);

void Heap::NetArray::init()
{
    Object::init();
    QV4::Scope scope(internalClass->engine);
    QV4::ScopedObject o(scope, this);
    o->setArrayType(Heap::ArrayData::Custom);
    o->defineAccessorProperty(QStringLiteral("length"), QV4::NetArray::method_length, nullptr);
    o->defineDefaultProperty(QStringLiteral("push"), QV4::NetArray::method_push);
    o->defineDefaultProperty(QStringLiteral("pop"), QV4::NetArray::method_push);
    object = scope.engine->memoryManager->m_persistentValues->allocate();
}

void Heap::NetArray::destroy()
{
    QV4::PersistentValueStorage::free(object);
    Object::destroy();
}

ReturnedValue NetArray::create(ExecutionEngine *engine, NetValue* netValue)
{
    Scope scope(engine);
    Scoped<NetArray> r(scope, engine->memoryManager->allocObject<NetArray>());
    *r->d()->object = QObjectWrapper::wrap(scope.engine, netValue);
    return r.asReturnedValue();
}

ReturnedValue NetArray::method_length(const FunctionObject *b, const Value *thisObject, const Value *argv, int argc)
{
    Scope scope(b);

    Scoped<QV4::NetArray> netArray(scope, thisObject->as<QV4::NetArray>());
    Scoped<QV4::QObjectWrapper> wrapper(scope, netArray->d()->object);
    if (!wrapper) {
        THROW_GENERIC_ERROR("No reference to the wrapped QObject exists.");
    }

    NetValue* netValue = reinterpret_cast<NetValue*>(wrapper->d()->object());
    QSharedPointer<NetTypeArrayFacade> arrayFacade = netValue->getNetReference()->getTypeInfo()->getArrayFacade();

    if(arrayFacade == nullptr) {
        THROW_GENERIC_ERROR("The wrapped object can't be treated as an array.");
    }

    return Encode(arrayFacade->getLength(netValue->getNetReference()));
}

ReturnedValue NetArray::method_push(const FunctionObject *b, const Value *thisObject, const Value *argv, int argc)
{
    Scope scope(b);

    Scoped<QV4::NetArray> netArray(scope, thisObject->as<QV4::NetArray>());
    Scoped<QV4::QObjectWrapper> wrapper(scope, netArray->d()->object);
    if (!wrapper) {
        THROW_GENERIC_ERROR("No reference to the wrapped QObject exists.");
    }

    NetValue* netValue = reinterpret_cast<NetValue*>(wrapper->d()->object());
    QSharedPointer<NetTypeArrayFacade> arrayFacade = netValue->getNetReference()->getTypeInfo()->getArrayFacade();

    if(arrayFacade == nullptr) {
        THROW_GENERIC_ERROR("The wrapped object can't be treated as an array.");
    }

    if(arrayFacade->isFixed()) {
        THROW_GENERIC_ERROR("Can't modify a fixed .NET list type.");
    }

    THROW_GENERIC_ERROR("TODO");
}

ReturnedValue NetArray::method_pop(const FunctionObject *b, const Value *thisObject, const Value *argv, int argc)
{
    Scope scope(b);

    Scoped<QV4::NetArray> netArray(scope, thisObject->as<QV4::NetArray>());
    Scoped<QV4::QObjectWrapper> wrapper(scope, netArray->d()->object);
    if (!wrapper) {
        THROW_GENERIC_ERROR("No reference to the wrapped QObject exists.");
    }

    NetValue* netValue = reinterpret_cast<NetValue*>(wrapper->d()->object());
    QSharedPointer<NetTypeArrayFacade> arrayFacade = netValue->getNetReference()->getTypeInfo()->getArrayFacade();

    if(arrayFacade == nullptr) {
        THROW_GENERIC_ERROR("The wrapped object can't be treated as an array.");
    }

    if(arrayFacade->isFixed()) {
        THROW_GENERIC_ERROR("Can't modify a fixed .NET list type.");
    }

    THROW_GENERIC_ERROR("TODO");
}

ReturnedValue NetArray::getIndexed(const Managed *m, uint index, bool *hasProperty)
{
    const NetArray *netArray = static_cast<const NetArray*>(m);
    Scope scope(netArray->engine());
    if(hasProperty)
        *hasProperty = true;

    QV4::Scoped<QV4::QObjectWrapper> wrapper(scope, netArray->d()->object);
    if (!wrapper) {
        THROW_GENERIC_ERROR("No reference to the wrapped QObject exists.");
    }

    NetValue* netValue = reinterpret_cast<NetValue*>(wrapper->d()->object());
    QSharedPointer<NetTypeArrayFacade> arrayFacade = netValue->getNetReference()->getTypeInfo()->getArrayFacade();

    if(arrayFacade == nullptr) {
        THROW_GENERIC_ERROR("The wrapped object can't be treated as an array.");
    }

    QSharedPointer<NetVariant> result = arrayFacade->getIndexed(netValue->getNetReference(), static_cast<int>(index));
    QJSValue resultJsValue = result->toQJSValue(scope.engine->jsEngine());
    return scope.engine->fromVariant(resultJsValue.toVariant());
}

bool NetArray::putIndexed(Managed *m, uint index, const Value &value)
{
    const NetArray *netArray = static_cast<const NetArray*>(m);
    Scope scope(netArray->engine());
    QV4::ScopedValue valueScope(scope, value);
    QJSValue valueJsValue(scope.engine, valueScope->asReturnedValue());

    QV4::Scoped<QV4::QObjectWrapper> wrapper(scope, netArray->d()->object);
    if (!wrapper) {
        THROW_GENERIC_ERROR("No reference to the wrapped QObject exists.");
    }

    NetValue* netValue = reinterpret_cast<NetValue*>(wrapper->d()->object());
    QSharedPointer<NetTypeArrayFacade> arrayFacade = netValue->getNetReference()->getTypeInfo()->getArrayFacade();

    if(arrayFacade == nullptr) {
        THROW_GENERIC_ERROR("The wrapped object can't be treated as an array.");
    }

    arrayFacade->setIndexed(netValue->getNetReference(),
                            static_cast<int>(index),
                            NetVariant::fromQJSValue(valueJsValue));
    return Encode::undefined();
}
