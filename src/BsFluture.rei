type t('e, 'v);

type voidFn = unit => unit;

type cancel =
  | NoCancel
  | Cancel(voidFn);

type cancelJs = (. unit) => unit;

type computation('e, 'v) = ('e => unit, 'v => unit) => cancel;

type nodeback('e, 'v) = (Js.null('e), option('v)) => unit;

let make: computation('e, 'v) => t('e, 'v);

let resolve: 'v => t('e, 'v);
let reject: 'e => t('e, 'v);

let after: (int, 'v) => t('e, 'v);
let rejectAfter: (int, 'e) => t('e, 'v);

let attempt: (unit => 'v) => t(Js.Exn.t, 'v);
let attemptP: (unit => Js.Promise.t('v)) => t('e, 'v);
let node: (nodeback('e, 'v) => unit) => t('e, 'v);
let encase: ('a => 'v, 'a) => t(Js.Exn.t, 'v);
let encaseP: ('a => Js.Promise.t('v), 'a) => t('e, 'v);

let map: ('a => 'b, t('e, 'a)) => t('e, 'b);
let mapRej: ('e => 'f, t('e, 'v)) => t('f, 'v);
let bimap: ('e => 'f, 'a => 'b, t('e, 'a)) => t('f, 'b);

let chain: ('a => t('f, 'b), t('e, 'a)) => t('f, 'b);
let chainRej: ('e => t('f, 'b), t('e, 'a)) => t('f, 'b);
let bichain: ('e => t('f, 'b), 'a => t('f, 'b), t('e, 'a)) => t('f, 'b);

let swap: t('e, 'v) => t('v, 'e);
let coalesce: ('e => 'a, 'v => 'a, t('e, 'v)) => t('f, 'a);

let fork: ('e => unit, 'v => unit, t('e, 'v)) => cancelJs;
let forkCatch:
  (Js.Exn.t => unit, 'e => unit, 'v => unit, t('e, 'v)) => cancelJs;
let value: ('v => unit, t('e, 'v)) => cancelJs;
let done_: (nodeback('e, 'v), t('e, 'v)) => cancelJs;
let promise: t('e, 'v) => Js.Promise.t('v);

let race: (t('e, 'v), t('e, 'v)) => t('e, 'v);
let both: (t('e, 'a), t('e, 'b)) => t('e, ('a, 'b));
let parallel: (int, array(t('e, 'v))) => t('e, array('v));

let isFuture: 'a => bool;
let never: t('e, 'v);
let isNever: t('e, 'v) => bool;

let let_: (t('e, 'a), 'a => t('e, 'b)) => t('e, 'b);
let and_: (t('e, 'a), t('e, 'b)) => t('e, ('a, 'b));
let try_: (t('e, 'v), 'e => t('f, 'v)) => t('f, 'v);
