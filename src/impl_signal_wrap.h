#ifndef IMPL_SIGNAL_WRAP_H
#define IMPL_SIGNAL_WRAP_H

namespace appjs {
class SignalBase {
public:
    SignalBase () { }
    virtual void emit() = 0;

/* Not to be implemented */
private:
    SignalBase (const SignalBase&);
    SignalBase& operator= (const SignalBase&);
};
} /* appjs */

#endif /* end of IMPL_SIGNAL_WRAP_H */
