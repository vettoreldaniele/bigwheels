#ifndef obj_ptr_h
#define obj_ptr_h

//! @class ObjPtrRef
//!
//!
template <typename ObjectT>
class ObjPtrRef
{
public:
    ObjPtrRef(ObjectT** ptrRef)
        : mPtrRef(ptrRef) {}

    ~ObjPtrRef() {}

    // clang-format off
    operator ObjectT** ()
    {
        return mPtrRef;
    }
    // clang-format on

    // clang-format off
    operator const ObjectT* const* ()
    {
        return mPtrRef;
    }
    // clang-format on

private:
    ObjectT** mPtrRef = nullptr;
};

//! @class ObjPtr
//!
//!
template <typename ObjectT>
class ObjPtr
{
public:
    ObjPtr(ObjectT* ptr = nullptr)
        : mPtr(ptr) {}

    ~ObjPtr() {}

    ObjPtr& operator=(const ObjPtr& rhs)
    {
        if (&rhs != this) {
            mPtr = rhs.mPtr;
        }
        return *this;
    }

    ObjPtr& operator=(const ObjectT* rhs)
    {
        if (rhs != mPtr) {
            mPtr = const_cast<ObjectT*>(rhs);
        }
        return *this;
    }

    operator bool() const
    {
        return (mPtr != nullptr);
    }

    bool operator==(const ObjPtr<ObjectT>& rhs) const
    {
        return mPtr == rhs.mPtr;
    }

    ObjectT* Get() const
    {
        return mPtr;
    }

    void Reset()
    {
        mPtr = nullptr;
    }

    bool IsNull() const
    {
        return mPtr == nullptr;
    }

    operator ObjectT*() const
    {
        return mPtr;
    }

    ObjectT* operator->() const
    {
        return mPtr;
    }

    ObjPtrRef<ObjectT> operator&()
    {
        return ObjPtrRef<ObjectT>(&mPtr);
    }

private:
    ObjectT* mPtr = nullptr;
};

#endif // obj_ptr_h