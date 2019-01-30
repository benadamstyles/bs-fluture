# bs-fluture

[ReasonML](https://reasonml.github.io/) bindings for [Fluture](https://github.com/fluture-js/Fluture).

This library of bindings is currently a work in progress. Please file an issue if one of the bindings is wrong or it isn't working for you in general.

## Justification

Futures, as provided by Fluture, give the following benefits over Promises:

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
  BsFluture.make((reject, resolve) => {
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

    /* EITHER return a wrapped cancellation function ... */
    BsFluture.Cancel(() => Js.Global.clearTimeout(timeoutId));
    /* ... OR return NoCancel */
    BsFluture.NoCancel;
  });

let cancelFuture =
  future
  |> BsFluture.fork(error => Js.log(error), response => Js.log(response));

/* Cancels a Future only if a cancellation function was provided */
BsFluture.safeCancel(cancelFuture);
```

## let-anything

This package is compatible with [let-anything](https://github.com/jaredly/let-anything) – use it like this:

```reason
let getDefaultUsername = (futureEmail: BsFluture.t(Js.Exn.t, string)) => {
  let%BsFluture email = futureEmail;
  let name = Js.String.split(email, "@")[0];
  BsFluture.resolve(name);
};
```
