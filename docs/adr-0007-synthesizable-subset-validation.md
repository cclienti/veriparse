# ADR-0007 — Synthesizable-subset validation (`SynthesizableCheck`)

- **Status**: Accepted — **implemented** (the pass, wired into `veriflat`, with the
  first rejected construct: virtual interfaces). The blacklist grows additively as
  more non-synthesizable grammar lands.
- **Date**: 2026-07-08
- **Scope**: A dedicated analysis pass that **rejects constructs outside the
  synthesizable RTL subset** before the transformation pipeline runs. Veriparse
  parses a broader slice of SystemVerilog than it can *transform* (the parser is
  deliberately permissive — ADR-0003 §1), so some legal, round-trippable SV has no
  meaning for `ModuleFlattener` and the other RTL passes. This pass is the single
  place that draws that line and fails loudly.
- **Normative reference** — IEEE 1800-2017, verified against `docs/1800-2017.pdf`:
  §25.9 (virtual interfaces) is the first rejected construct. Future entries cite
  their own clause.
- **Relationship to prior ADRs**:
  - **ADR-0002 §2.3/§6** parses `virtual` interface types into
    `InterfaceType{is_virtual}` because `virtual` is decisive syntax. That ADR
    listed virtual interfaces *in scope* for the **front end**; this ADR refines
    that: they are parsed and round-tripped, but **rejected for synthesis** here.
    (ADR-0002 §7 carries a back-reference.)
  - **ADR-0006** (name resolution) is the sibling pass that also emits located,
    clause-cited diagnostics for interface misuse (§25.5 modport placement,
    interface-as-data-type). This pass complements it: name resolution answers
    *"does this name bind legally?"*; this pass answers *"is this construct in the
    RTL subset?"*. They are kept separate on purpose (§2 below).

---

## 1. Why a dedicated pass, not a check bolted onto name resolution

The rejection could physically live in `NameResolution` (it already carries the
sibling interface checks). It does **not**, for two reasons:

1. **Single responsibility.** `NameResolution` binds names; synthesizability is a
   different question. Piling subset policy onto a resolution pass makes both
   harder to reason about and guarantees drift as the blacklist grows (every future
   non-synth construct would add an `if` to a pass that has nothing to do with
   synthesis).
2. **Per-tool opt-in.** Synthesizability is a policy of the *flattening* tool, not
   of name binding. A dedicated pass lets each front end choose:
   - `veriflat` **runs** it (its output must be synthesizable RTL);
   - `veridump` / `veriobf` **do not** — they round-trip or obfuscate *any* parsed
     SV, virtual interfaces included.
   Burying the check in a shared resolution pass would force the policy on every
   tool that merely needs name binding.

---

## 2. Decisions

1. **Blacklist now, whitelist later.** The pass rejects a **known set** of
   non-synthesizable constructs rather than asserting a closed *authorized* set.
   A whitelist ("only these constructs pass") is the true synthesizable-subset gate
   and the eventual target, but it is **brittle while the SV parser is still
   growing**: every legal-but-not-yet-whitelisted construct would be a false
   rejection, against a moving target. A blacklist starts as one check and grows
   only when non-synthesizable grammar is actually added. The migration to a
   whitelist is deferred to when parser coverage stabilizes.
2. **Read-only analysis pass.** It mutates nothing; it walks the design and returns
   the **count of violations** (0 ⇒ synthesizable). It lives in
   `Passes::Analysis` alongside `Module` / `UniqueDeclaration`.
3. **Full recursion, collect-all diagnostics.** The walk descends through **every**
   node (not a subtree-pruning search), and reports **all** violations in one pass
   before returning non-zero — a validation gate should not die on the first
   offender.
4. **Located, clause-cited errors.** Each violation is a `LOG_ERROR_N(node)` naming
   the construct and its IEEE clause, matching the house diagnostic style.

---

## 3. The blacklist

| Construct | Clause | Node signal | Status |
|---|---|---|---|
| Virtual interface | §25.9 | `InterfaceType.is_virtual == true` | **rejected** |
| Clocking blocks | §14 | (no grammar — natural parse error) | n/a here |
| Classes / covergroups / assertions / DPI | §8/§18/§16/§35 | future nodes | additive |

A virtual interface is a **dynamic handle** — a class/testbench construct with no
static hardware. It reaches this pass as an `InterfaceType` whose `is_virtual` flag
is set (decisive at parse, ADR-0002 §2.3), on a `Var` declaration or a port. There
is no flattening semantics for it, so the design is rejected rather than silently
mis-lowered.

Clocking blocks (§14) are **not** listed as an active check: `clocking` is not a
keyword, so it is already a syntax error at parse — an *absence* of grammar, not a
subset check. If clocking grammar is ever added (for round-tripping), it becomes a
blacklist entry here.

---

## 4. Placement in the pipeline

`veriflat` runs the pass **after `NameResolution`** — so any `NamedType` promoted to
`InterfaceType` is already visible — and **before** the module dictionary /
`ModuleFlattener`:

```
PackageInliner → NameResolution → SynthesizableCheck → ModuleFlattener → …
```

A non-zero return aborts the run with a single top-level error, before any
transformation touches a construct it cannot model.

The check runs over **every compilation unit** (`check(sources)`), not only the
tree reachable from the selected top module. This is deliberate: everything handed
to `veriflat` is "the design", and a non-synthesizable construct anywhere in it is
reported up front — even in a module the chosen top does not instantiate. The
alternative (gate only the flattened top, so an unused sibling module carrying a
`virtual` interface would not block the run) is more permissive but validates the
*output* rather than the *input*; the stricter whole-design gate is preferred, and
callers that want to flatten a synthesizable top out of a mixed file should split
the non-synthesizable modules into a separate file.

---

## 5. Extending the pass

Each new non-synthesizable construct adds one sub-check to `SynthesizableCheck`
(an anonymous-namespace walker in the `.cpp`), cites its clause, and gains a
rejection test. The public surface (`check(node)` / `check(node_list)`) does not
change. When the parser's synthesizable coverage is stable enough, the blacklist is
inverted into a whitelist (Decision 1) — a follow-up ADR at that point.
</invoke>
