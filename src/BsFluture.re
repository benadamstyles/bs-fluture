type t('e, 'v);

type unaryFn('a, 'b) = 'a => 'b;

type curriedBinaryFn('a, 'b, 'c) = unaryFn('a, unaryFn('b, 'c));

type curriedTernaryFn('a, 'b, 'c, 'd) =
  unaryFn('a, unaryFn('b, unaryFn('c, 'd)));

type curriedQuaternaryFn('a, 'b, 'c, 'd, 'e) =
  unaryFn('a, unaryFn('b, unaryFn('c, unaryFn('d, 'e))));

type voidFn = unaryFn(unit, unit);

type cancel =
  | NoCancel
  | Cancel(voidFn);

type computation('e, 'v) = ('e => unit, 'v => unit) => cancel;

type cancelJs = (. unit) => unit;

type nodeback('e, 'v) = (Js.null('e), option('v)) => unit;

[@bs.module "fluture"]
external make_: (('e => unit, 'v => unit) => cancelJs) => t('e, 'v) =
  "default";

let wrapComputation = (compute: computation('e, 'v), rej, res): cancelJs =>
  switch (compute(rej, res)) {
  | Cancel(cancel) => ((.) => cancel())
  | NoCancel => ((.) => ())
  };

let make = (compute: computation('e, 'v)) =>
  make_(wrapComputation(compute));

[@bs.module "fluture"] external resolve: 'v => t('e, 'v) = "resolve";

[@bs.module "fluture"] external reject: 'e => t('e, 'v) = "reject";

[@bs.module "fluture"]
external after: curriedBinaryFn(int, 'v, t('e, 'v)) = "after";

[@bs.module "fluture"]
external rejectAfter: curriedBinaryFn(int, 'e, t('e, 'v)) = "rejectAfter";

[@bs.module "fluture"]
external attempt: (unit => 'v) => t(Js.Exn.t, 'v) = "attempt";

[@bs.module "fluture"]
external attemptP: (unit => Js.Promise.t('v)) => t('e, 'v) = "attemptP";

[@bs.module "fluture"]
external node: (nodeback('e, 'v) => unit) => t('e, 'v) = "node";

[@bs.module "fluture"]
external encase: curriedBinaryFn('a => 'v, 'a, t(Js.Exn.t, 'v)) = "encase";

[@bs.module "fluture"]
external encaseP: curriedBinaryFn('a => Js.Promise.t('v), 'a, t('e, 'v)) =
  "encaseP";

[@bs.module "fluture"]
external map: curriedBinaryFn('a => 'b, t('e, 'a), t('e, 'b)) = "map";

[@bs.module "fluture"]
external mapRej: curriedBinaryFn('e => 'f, t('e, 'v), t('f, 'v)) = "mapRej";

[@bs.module "fluture"]
external bimap: curriedTernaryFn('e => 'f, 'a => 'b, t('e, 'a), t('f, 'b)) =
  "bimap";

[@bs.module "fluture"]
external chain: curriedBinaryFn('a => t('f, 'b), t('e, 'a), t('f, 'b)) =
  "chain";

[@bs.module "fluture"]
external chainRej: curriedBinaryFn('e => t('f, 'b), t('e, 'a), t('f, 'b)) =
  "chainRej";

[@bs.module "fluture"]
external bichain:
  curriedTernaryFn('e => t('f, 'b), 'a => t('f, 'b), t('e, 'a), t('f, 'b)) =
  "bichain";

[@bs.module "fluture"] external swap: t('e, 'v) => t('v, 'e) = "swap";

[@bs.module "fluture"]
external coalesce:
  curriedTernaryFn('e => 'a, 'v => 'a, t('e, 'v), t('f, 'a)) =
  "coalesce";

[@bs.module "fluture"]
external fork: curriedTernaryFn('e => unit, 'v => unit, t('e, 'v), cancelJs) =
  "fork";

[@bs.module "fluture"]
external forkCatch:
  curriedQuaternaryFn(
    Js.Exn.t => unit,
    'e => unit,
    'v => unit,
    t('e, 'v),
    cancelJs,
  ) =
  "forkCatch";

[@bs.module "fluture"]
external value: curriedBinaryFn('v => unit, t('e, 'v), cancelJs) = "value";

[@bs.module "fluture"]
external done_: curriedBinaryFn(nodeback('e, 'v), t('e, 'v), cancelJs) =
  "done";

[@bs.module "fluture"]
external promise: t('e, 'v) => Js.Promise.t('v) = "promise";

[@bs.module "fluture"]
external race: curriedBinaryFn(t('e, 'v), t('e, 'v), t('e, 'v)) = "race";

[@bs.module "fluture"]
external both: curriedBinaryFn(t('e, 'a), t('e, 'b), t('e, ('a, 'b))) =
  "both";

[@bs.module "fluture"]
external parallel:
  curriedBinaryFn(int, array(t('e, 'v)), t('e, array('v))) =
  "parallel";

[@bs.module "fluture"] external isFuture: 'a => bool = "isFuture";

[@bs.module "fluture"] external never: t('e, 'v) = "never";

[@bs.module "fluture"] external isNever: t('e, 'v) => bool = "isNever";

let let_ = (future, continuation) => future |> chain(continuation);
let and_ = both;
let try_ = (future, handler) => future |> chainRej(handler);
