type t('e, 'v);

type cancel =
  | NoCancel
  | Cancel(unit => unit);

type cancelJs;

type computation('e, 'v) = ('e => unit, 'v => unit) => cancel;

type nodeback('e, 'v) = ('e, 'v) => unit;

let safeCancel: cancelJs => unit;

let make: computation('e, 'v) => t('e, 'v);
let resolve: 'v => t(unit, 'v);
let reject: 'e => t('e, unit);
let after: (int, 'v) => t(unit, 'v);
let rejectAfter: (int, 'e) => t('e, unit);
let attempt: (unit => 'v) => t(Js.Exn.t, 'v);
let tryP: (unit => Js.Promise.t('v)) => t('e, 'v);
let node: (nodeback('e, 'v) => unit) => t('e, 'v);
let encase: ('a => 'v, 'a) => t(Js.Exn.t, 'v);
let encase2: (('a, 'b) => 'v, 'a, 'b) => t(Js.Exn.t, 'v);
let encase3: (('a, 'b, 'c) => 'v, 'a, 'b, 'c) => t(Js.Exn.t, 'v);
let encaseP: ('a => Js.Promise.t('v), 'a) => t(Js.Exn.t, 'v);
let encaseP2: (('a, 'b) => Js.Promise.t('v), 'a, 'b) => t(Js.Exn.t, 'v);
let encaseP3:
  (('a, 'b, 'c) => Js.Promise.t('v), 'a, 'b, 'c) => t(Js.Exn.t, 'v);
let encaseN: (('a, nodeback('e, 'v)) => unit, 'a) => t('e, 'v);
let encaseN2: (('a, 'b, nodeback('e, 'v)) => unit, 'a, 'b) => t('e, 'v);
let encaseN3:
  (('a, 'b, 'c, nodeback('e, 'v)) => unit, 'a, 'b, 'c) => t('e, 'v);
let map: ('a => 'b, t('e, 'a)) => t('e, 'b);
let mapRej: ('e => 'f, t('e, 'v)) => t('f, 'v);
let bimap: ('e => 'f, 'a => 'b, t('e, 'a)) => t('f, 'b);
let chain: ('a => t('e, 'b), t('e, 'a)) => t('e, 'b);
let chainRej: ('e => t('f, 'v), t('e, 'v)) => t('f, 'v);
let swap: t('e, 'v) => t('v, 'e);
let fold: ('e => 'a, 'v => 'a, t('e, 'v)) => t(unit, 'a);
let fork: ('e => unit, 'v => unit, t('e, 'v)) => cancelJs;
let forkCatch:
  (Js.Exn.t => unit, 'e => unit, 'v => unit, t('e, 'v)) => cancelJs;
let value: ('v => unit, t(unit, 'v)) => cancelJs;
let done_: (nodeback('e, 'v), t('e, 'v)) => cancelJs;
let promise: t('e, 'v) => Js.Promise.t('v);
let race: (t('e, 'v), t('e, 'v)) => t('e, 'v);
let both: (t('e, 'a), t('e, 'b)) => t('e, ('a, 'b));
let parallel: (int, array(t('e, 'v))) => t('e, array('v));
let isFuture: 'a => bool;
let never: unit => t(unit, unit);
let isNever: 'a => bool;
