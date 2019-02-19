type t('e, 'v);

type cancel =
  | NoCancel
  | Cancel(unit => unit);

type computation('e, 'v) = ('e => unit, 'v => unit) => cancel;

type cancelJs = option((. unit) => unit);

let safeCancel: cancelJs => unit =
  fun
  | Some(c) => c(.)
  | None => ();

type nodeback('e, 'v) = (Js.nullable('e), 'v) => unit;

/**
 * Creating
 */
[@bs.module "fluture"]
external make_: (('e => unit, 'v => unit) => cancelJs) => t('e, 'v) =
  "default";

let wrapComputation = (compute: computation('e, 'v), rej, res): cancelJs =>
  switch (compute(rej, res)) {
  | Cancel(c) => Some(((.) => c()))
  | NoCancel => None
  };

let make = (compute: computation('e, 'v)) =>
  make_(wrapComputation(compute));

[@bs.module "fluture"] external resolve: 'v => t('e, 'v) = "of";

[@bs.module "fluture"] external reject: 'e => t('e, 'v) = "";

[@bs.module "fluture"] external after: (int, 'v) => t('e, 'v) = "";

[@bs.module "fluture"] external rejectAfter: (int, 'e) => t('e, 'v) = "";

[@bs.module "fluture"]
external attempt: (unit => 'v) => t(Js.Exn.t, 'v) = "try";

[@bs.module "fluture"]
external tryP: (unit => Js.Promise.t('v)) => t('e, 'v) = "";

[@bs.module "fluture"]
external node: (nodeback('e, 'v) => unit) => t('e, 'v) = "";

[@bs.module "fluture"]
external encase: ('a => 'v, 'a) => t(Js.Exn.t, 'v) = "";

[@bs.module "fluture"]
external encase2: (('a, 'b) => 'v, 'a, 'b) => t(Js.Exn.t, 'v) = "";

[@bs.module "fluture"]
external encase3: (('a, 'b, 'c) => 'v, 'a, 'b, 'c) => t(Js.Exn.t, 'v) = "";

[@bs.module "fluture"]
external encaseP: ('a => Js.Promise.t('v), 'a) => t(Js.Exn.t, 'v) = "";

[@bs.module "fluture"]
external encaseP2: (('a, 'b) => Js.Promise.t('v), 'a, 'b) => t(Js.Exn.t, 'v) =
  "";

[@bs.module "fluture"]
external encaseP3:
  (('a, 'b, 'c) => Js.Promise.t('v), 'a, 'b, 'c) => t(Js.Exn.t, 'v) =
  "";

[@bs.module "fluture"]
external encaseN: (('a, nodeback('e, 'v)) => unit, 'a) => t('e, 'v) = "";

[@bs.module "fluture"]
external encaseN2: (('a, 'b, nodeback('e, 'v)) => unit, 'a, 'b) => t('e, 'v) =
  "";

[@bs.module "fluture"]
external encaseN3:
  (('a, 'b, 'c, nodeback('e, 'v)) => unit, 'a, 'b, 'c) => t('e, 'v) =
  "";

/**
 * Transforming
 */
[@bs.module "fluture"]
external map: ('a => 'b, t('e, 'a)) => t('e, 'b) = "";

[@bs.module "fluture"]
external mapRej: ('e => 'f, t('e, 'v)) => t('f, 'v) = "";

[@bs.module "fluture"]
external bimap: ('e => 'f, 'a => 'b, t('e, 'a)) => t('f, 'b) = "";

[@bs.module "fluture"]
external chain: ('a => t('e, 'b), t('e, 'a)) => t('e, 'b) = "";

[@bs.module "fluture"]
external chainRej: ('e => t('f, 'v), t('e, 'v)) => t('f, 'v) = "";

[@bs.module "fluture"] external swap: t('e, 'v) => t('v, 'e) = "";

[@bs.module "fluture"]
external fold: ('e => 'a, 'v => 'a, t('e, 'v)) => t('f, 'a) = "";

/**
 * Consuming
 */
[@bs.module "fluture"]
external fork: ('e => unit, 'v => unit, t('e, 'v)) => cancelJs = "";

[@bs.module "fluture"]
external forkCatch:
  (Js.Exn.t => unit, 'e => unit, 'v => unit, t('e, 'v)) => cancelJs =
  "";

[@bs.module "fluture"]
external value: ('v => unit, t('e, 'v)) => cancelJs = "";

[@bs.module "fluture"]
external done_: (nodeback('e, 'v), t('e, 'v)) => cancelJs = "done";

[@bs.module "fluture"] external promise: t('e, 'v) => Js.Promise.t('v) = "";

/**
 * Parallelism
 */
[@bs.module "fluture"]
external race: (t('e, 'v), t('e, 'v)) => t('e, 'v) = "";

[@bs.module "fluture"]
external both: (t('e, 'a), t('e, 'b)) => t('e, ('a, 'b)) = "";

[@bs.module "fluture"]
external parallel: (int, array(t('e, 'v))) => t('e, array('v)) = "";

/**
 * Utility
 */ [@bs.module "fluture"] external isFuture: 'a => bool = "";

[@bs.module "fluture"] external never: unit => t('e, 'v) = "";

[@bs.module "fluture"] external isNever: 'a => bool = "";

let let_ = (future, continuation) => future |> chain(continuation);
let and_ = both;
let try_ = (future, handler) => future |> chainRej(handler);
