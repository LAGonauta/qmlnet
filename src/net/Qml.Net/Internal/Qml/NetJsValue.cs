﻿using System;
using System.Dynamic;
using System.Runtime.InteropServices;
using Qml.Net.Internal.PInvoke;

namespace Qml.Net.Internal.Qml
{
    internal class NetJsValue : BaseDisposable
    {
        public NetJsValue(IntPtr handle, bool ownsHandle = true)
            : base(handle, ownsHandle)
        {
            
        }

        public bool IsCallable => Interop.NetJsValue.IsCallable(Handle);
        
        public NetVariant Call(NetVariantList parameters)
        {
            var result = Interop.NetJsValue.Call(Handle, parameters?.Handle ?? IntPtr.Zero);
            return result != IntPtr.Zero ? new NetVariant(result) : null;
        }
        
        public object Call(params object[] parameters)
        {
            NetVariantList variants = null;
            
            if (parameters != null && parameters.Length > 0)
            {
                variants = new NetVariantList();
                foreach (var parameter in parameters)
                {
                    using (var variant = new NetVariant())
                    {
                        Helpers.PackValue(parameter, variant);
                        variants.Add(variant);
                    }
                }
            }
            
            var result = Call(variants);
            
            variants?.Dispose();

            if (result == null)
            {
                return null;
            }
            
            object returnValue = null;
            Helpers.Unpackvalue(ref returnValue, result);
            result.Dispose();
            
            return returnValue;
        }

        public NetVariant GetProperty(string propertyName)
        {
            var result = Interop.NetJsValue.GetProperty(Handle, propertyName);
            if (result == IntPtr.Zero)
            {
                return null;
            }
            return new NetVariant(result);
        }

        public void SetProperty(string propertyName, NetVariant value)
        {
            Interop.NetJsValue.SetProperty(Handle, propertyName, value?.Handle ?? IntPtr.Zero);
        }
        
        protected override void DisposeUnmanaged(IntPtr ptr)
        {
            Interop.NetVariant.Destroy(ptr);
        }

        public dynamic AsDynamic()
        {
            return new NetJsValueDynamic(this);
        }

        internal class NetJsValueDynamic : DynamicObject, INetJsValue
        {
            readonly NetJsValue _jsValue;

            public NetJsValueDynamic(NetJsValue jsValue)
            {
                _jsValue = jsValue;
            }

            public void Dispose()
            {
                _jsValue.Dispose();
            }

            public NetJsValue JsValue => _jsValue;
            
            public bool IsCallable => _jsValue.IsCallable;
            
            public object Call(params object[] parameters)
            {
                return _jsValue.Call(parameters);
            }

            public override bool TryInvoke(InvokeBinder binder, object[] args, out object result)
            {
                result = null;
                
                if (!IsCallable)
                {
                    return false;
                }

                result = Call(args);
                
                return true;
            }

            public override bool TryGetMember(GetMemberBinder binder, out object result)
            {
                var property = _jsValue.GetProperty(binder.Name);
                if (property == null)
                {
                    result = null;
                }
                else
                {
                    object unpacked = null;
                    Helpers.Unpackvalue(ref unpacked, property);
                    result = unpacked;
                }
                return true;
            }

            public override bool TrySetMember(SetMemberBinder binder, object value)
            {
                if (value == null)
                {
                    _jsValue.SetProperty(binder.Name, null);
                }
                else
                {
                    using (var variant = new NetVariant())
                    {
                        Helpers.PackValue(value, variant);
                        _jsValue.SetProperty(binder.Name, variant);
                    }
                }

                return true;
            }
        }
    }

    public interface INetJsValue : IDisposable
    {
        bool IsCallable { get; }

        object Call(params object[] parameters);
    }
    
    internal interface INetJsValueInterop : IPinvoke
    {
        [NativeSymbol(Entrypoint = "net_js_value_destroy")]
        void Destroy(IntPtr jsValue);

        [NativeSymbol(Entrypoint = "net_js_value_isCallable")]
        bool IsCallable(IntPtr jsValue);
        [NativeSymbol(Entrypoint = "net_js_value_call")]
        IntPtr Call(IntPtr jsValue, IntPtr parameters);
        [NativeSymbol(Entrypoint = "net_js_value_getProperty")]
        IntPtr GetProperty(IntPtr jsValue, [MarshalAs(UnmanagedType.LPWStr)] string propertyName);
        [NativeSymbol(Entrypoint = "net_js_value_setProperty")]
        void SetProperty(IntPtr jsValue, [MarshalAs(UnmanagedType.LPWStr)] string propertyName, IntPtr value);
    }
}