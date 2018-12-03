# bs-fluture

[ReasonML](https://reasonml.github.io/) bindings for [Fluture](https://github.com/fluture-js/Fluture).

This library of bindings is currently a work in progress. Please file an issue if one of the bindings is wrong or it isn't working for you in general.

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
    BsFluture.Cancel(
      (.) => Js.Global.clearTimeout(timeoutId) /* see below */,
    );
    /* ... OR return NoCancel */
    BsFluture.NoCancel;
  });

let cancelFuture =
  future
  |> BsFluture.fork(error => Js.log(error), response => Js.log(response));

/* Cancels a Future only if a cancellation function was provided */
BsFluture.safeCancel(cancelFuture);
```

### Cancellation

**TL;DR:** Due to an incompatibility between how BuckleScript compiles functions and the runtime type-checking of Fluture, you currently need to explicitly uncurry your cancellation function.

#### The explanation:

Bucklescript compiles this:

```reason
() => true;
```

to something like this:

```js
function(param) {
  return true;
}
```

However, Fluture.js type checks your cancellation functions at runtime so if you returned the above function from your `BsFluture.make()` function, you'd get the following error:

```
TypeError: The computation was expected to return a nullary function or void
  Actual: function (param) {
      return true;
    }
```

It is complaining because it wants a nullary function (a function that takes no arguments), but your compiled function takes a single argument, `param`, which it then ignores. If instead you uncurry your nullary function like this:

```reason
(.) => true;
```

then BuckleScript compiles it thus:

```js
function() {
  return true;
}
```

and all is well.
