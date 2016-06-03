/*
Copyright (c) <2016> <Hyun Park>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once
/*
	Dynamic Delegate
*/
template<typename Ret=void,typename... Args> ref class DynamicDelegate {
	typedef Ret(*FunctionPtr)(Args...);
	delegate Ret Delegator(Args...);
	typedef DynamicDelegate<Ret,Args...> Base;
	Delegator^ delegator;
	GCHandle Handle;
	IntPtr HandlePtr;
public:
	/* constructors */
	DynamicDelegate(Delegate^ delegator) {
		Attach(delegator);
	}
	DynamicDelegate(Object^ target, String^ Method) : DynamicDelegate(Delegator::CreateDelegate(Delegator::typeid, target, Method)) {}

	DynamicDelegate(Base% other) : delegator(other.delegator),Handle(other.Handle),HandlePtr(other.HandlePtr) {}

	!DynamicDelegate() {
		//detach();
	}

	~DynamicDelegate() { DynamicDelegate::!DynamicDelegate(); }

	/* Managements */

	void Attach() {
		Attach(nullptr);
	}
	void Attach(Delegate^ target) {
		if (this->delegator != nullptr) {
			detach();
		}
		if (target != nullptr) {
			this->delegator = (Delegator^)target;
			this->Handle = GCHandle::Alloc(this->delegator);
			this->HandlePtr = Marshal::GetFunctionPointerForDelegate(this->delegator);
		}
	}
	void detach() {
		if (this->Handle.IsAllocated) {
			this->Handle.Free();
		}
		this->delegator = nullptr;
		this->HandlePtr = IntPtr::Zero;

	}
	/* Functions */
	Object^ Clone() {
		return delegator->Clone();
	}
	Object^ DynamicInvoke(...cli::array<Object^>^ args) {
		return delegator->DynamicInvoke(args);
	}

	Ret Invoke(Args... args) {
		return delegator->Invoke(args...);
	}
	/* Properties */
	property Object^ Target {
		Object^ get() { return delegator->Target; }
	}
	property System::Reflection::MethodInfo^ Method {
		System::Reflection::MethodInfo^ get() { return delegator->Method; }
	}
	
	/* Convert Operations */
	operator Delegate ^ () {
		return delegator;
	}

	static operator const FunctionPtr(Base% rhs){
		return rhs.get();
	}
	Ret operator ()(Args... args) {
		Invoke(args...);
	}
	const FunctionPtr get() {
		return static_cast<FunctionPtr>(HandlePtr.ToPointer());
	}

	/* Assign Operation */
	Base% operator =(Base% rhs) {
		if (this != %rhs) {
			attach(rhs->delegator);
		}
		return *this;
	}

	/* Equal Operations */
	bool operator == (Base% rhs) {
		return this->delegator == rhs.delegator;
	}
	bool operator!() {
		return delegator == nullptr;
	}
};