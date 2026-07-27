# ADR-0013 — Assignment operators & increment/decrement desugar at parse time

- **Status**: Accepted — implemented on this branch.
- **Date**: 2026-07-27
- **Scope**: SystemVerilog **operator assignments** (IEEE 1800-2017 §11.4.1:
  `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`, `<<<=`,
  `>>>=`) and **increment/decrement** (§11.4.2: `i++`, `i--`, `++i`, `--i`)
  in the two places the synthesizable subset uses them: **blocking
  statement position** and **for-loop steps** (procedural and generate).
  All of it is SV-only — none of these forms exist in 1364-2005, so the
  tokens are gated on SV mode and Verilog-mode lexing is unchanged.
- **Normative reference** — IEEE 1800-2017 §11.4.1, §11.4.2, verified
  against `docs/1800-2017.pdf`: an operator assignment is
  *"semantically equivalent to a blocking assignment, with the
  exception that any left-hand index expression is only evaluated
  once"* (`a[i] += 2;` same as `a[i] = a[i] + 2;` — the clause's own
  example is the expansion); increment/decrement *"behave as blocking
  assignments"*. A.6.8: `for_step_assignment ::= operator_assignment |
  inc_or_dec_expression | …`.

## 1. Decision — desugar in the parser, no new AST nodes

The parser builds a plain `BlockingSubstitution` whose right-hand side is
the operator node applied to a **clone of the lvalue's expression** and
the written RHS: `i <<= a + b` parses as `i = i << (a + b)` (the tree
shape needs no parenthesization — precedence is fixed by construction).
This extends the existing precedent: the for-step `i++`/`i--` productions
have desugared this way since they were introduced.

Why this does **not** violate ADR-0003's "record syntax faithfully"
principle: that principle is about *deferring what syntax cannot decide*
(an implicit type, an ambiguous name) as an honest neutral node. An
operator assignment is **unambiguous sugar** — the clause itself defines
it *by* its expansion, no later pass could decide anything differently,
and no information a downstream pass needs is lost. What is lost is only
the spelling: `veridump` renders `i += 1` back as `i = i + 1`. That
trade was already made for `i++` in for-steps; this ADR makes it uniform
and records it.

Operator-to-node mapping (same nodes the binary expressions build):
`+=`→`Plus`, `-=`→`Minus`, `*=`→`Times`, `/=`→`Divide`, `%=`→`Mod`,
`&=`→`And`, `|=`→`Or`, `^=`→`Xor`, `<<=`→`Sll`, `>>=`→`Srl`,
`<<<=`→`Sll`, `>>>=`→`Sra` (`<<<`/`<<` build the same node in
expressions, and §11.4.1 defines the compound forms by those operators).

## 2. Decision — real tokens, maximal munch, SV-gated

The operators are lexed as single tokens (`+=`, `++`, …), added ahead of
their prefixes with the standard longest-match rule. In Verilog mode
each rule falls back (`yyless`) to the 1364 tokenization, so `a++b`
still lexes as `a + (+b)` there. In SV mode `a++b` lexes as `a ++ b`
and errors — the same maximal-munch behavior as every SV tool; write
`a + +b`. The alternative (two-token grammar productions, no lexer
change) was rejected: it silently accepts `i + = 1`, and the compound
family is large enough that the leniency would be systematic rather
than a single legacy quirk (the old two-token `i + +` for-step
productions are kept for Verilog-mode compatibility only).

## 3. The index-evaluated-once caveat — why it is moot here

§11.4.1 evaluates a left-hand **index expression** once; the expansion
clones the whole lvalue, evaluating any index twice. The two diverge
only when evaluating the index has a side effect — i.e. only for
**expression-embedded** inc/dec (`a[i++] += x`), which this ADR
deliberately does not admit (§4). For every admitted form the lvalue is
a plain variable/select whose double evaluation is observationally
identical — exactly the clause's own `a[i] += 2` ⇔ `a[i] = a[i] + 2`
example.

## 4. Not in scope

- **Expression-embedded inc/dec** (`a[i++]`, `x = y++`) — §11.4.2 allows
  them, the synthesizable subset does not need them, and they are the
  one place the clone-based expansion would be wrong. They stay syntax
  errors.
- **Operator assignment in for-init** (`for (i += 1; …)`) — not legal:
  §12.7.1 for_initialization admits only plain assignments.
- **Nonblocking compound forms** — do not exist in the standard.
- **Delay-annotated compound assignments** (`#1 i += 1;`) — legal per
  A.6.2 (operator_assignment has no intra-assignment delay, but a
  statement delay may precede); deferred until wanted.

## 5. Tests

Parser goldens: every operator as a statement (module process, function
and task bodies), the four inc/dec statement forms, compound and
inc/dec for-steps (procedural and generate), each locked to its
desugared AST; Verilog-mode lexing of `a++b`/`i + +` unchanged by
construction (SV-gated rules) and covered by the existing suites.
