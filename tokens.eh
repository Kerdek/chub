export JID  () none
export JPCT () none
export JCHR () none
export JSTR () none
export JNUM () none

type
  TokenKind @() none

  Token {
    s @Token
    q TokenKind
    p @byte
    n nat
    line nat
    col nat
  }

  K @Token

  Ctx {
    c0    byte
    c1    byte
    k    @K
    p    @byte
    line  nat
    col   nat
  }

  X @Ctx
;

export token (x X) none
