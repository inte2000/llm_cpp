#ifndef __SCOPE_GUARD_H__
#define __SCOPE_GUARD_H__

#include <type_traits>

template<typename T>
concept NonReferenceType = !std::is_reference_v<T>;

template<NonReferenceType Func>
class ScopeGuard final {
public:

    explicit ScopeGuard(Func f)
        : m_Func(std::forward<Func>(f)), m_isOnGuard(true) {
    }

    ~ScopeGuard() noexcept {
        try {
            if (m_isOnGuard) {
                m_Func();
                m_isOnGuard = false;
            }
        }
        catch (...) {
        }
    }

    ScopeGuard(const ScopeGuard<Func>&) = delete;
    ScopeGuard(ScopeGuard<Func>&&) = delete;
    ScopeGuard<Func>& operator = (const ScopeGuard<Func>&) = delete;
    ScopeGuard<Func>& operator = (ScopeGuard<Func>&&) = delete;

    void Dismiss() noexcept {
        m_isOnGuard = false;
    }
private:
    Func m_Func;
    bool m_isOnGuard{ false };
};

/*
void SomeFunc()
{
    FILE* fp = fopen("filename", "wb");

    ScopeGuard< std::function<void()> > guard([&fp]() { fclose(fp); });
    //现在可以安全地使用 fp 文件指针了
}
*/
#endif //ifndef __SCOPE_GUARD_H__
