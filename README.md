# Fluture for ReScript / ReasonML / BuckleScript

[ReasonML](https://reasonml.github.io/) bindings for [Fluture](https://github.com/fluture-js/Fluture).

> **NOTE:** v1+ of `bs-fluture` is only compatible with v12+ of Fluture (see v12's [Breaking Changes document](https://gist.github.com/Avaq/ee2c6c819db4c37258e9a226e6380a38#functions-use-simple-currying) for more details).
> If you are using v11 of Fluture or lower, you must use v0.x of `bs-fluture`.

This library of bindings does not currently cover 100% of the Fluture API, although it does cover the vast majority of the commonly-used API, especially for the ReScript/ReasonML context. Please file an issue if one of the bindings is missing, wrong or it isn't working for you in general.

## Justification

Futures give the following benefits over Promises:

- Futures are lazy, rather than eager. Creating a future doesn't actually perform the async task – that only happens when you consume the future with e.g. `fork()`. Promises, on the other hand, perform their async task as soon as they are created. If you want a Promise that you can pass around without actually running it, you have to wrap it in a function, introducing more boilerplate.
- Cancellation is a built-in feature of futures.
- Unlike Promises (even in Reason!), Futures explicitly type their error states.

## Installation

```sh
npm install --save bs-fluture
```

Then add bs-fluture to bs-dependencies in your `bsconfig.json`:

```json
{
  "bs-dependencies": ["bs-fluture"]
}
```

## Usage

```reason
let future =
  Fluture.make((reject, resolve) => {
    let timeoutId =
      Js.Global.setTimeout(
        () =>
          if (Random.bool()) {
            resolve("success");
          } else {
            reject("error");
          },
        30,
      );

    /** EITHER return a wrapped cancellation function ... */
    Fluture.Cancel(() => Js.Global.clearTimeout(timeoutId));
    /** ... OR return NoCancel if there is no way to clean up. */
    Fluture.NoCancel;
  });

let cancelFuture =
  future
  |> Fluture.fork(error => Js.Console.error(error), response => Js.log(response));

/**
 * Due to runtime type-checking in Fluture,
 * the cancellation function must be explicitly uncurried.
 */
cancelFuture(.);
```

## let-anything

This package is compatible with [let-anything](https://github.com/jaredly/let-anything) – use it like this:

```reason
let getDefaultUsername = (futureEmail: Fluture.t(Js.Exn.t, string)) => {
  let%Fluture email = futureEmail;
  let name = Js.String.split(email, "@")[0];
  Fluture.resolve(name);
};
```
