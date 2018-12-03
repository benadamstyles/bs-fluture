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
let fetchData = () =>
  BsFluture.encaseP(fetch, "https://example.com")
  |> BsFluture.fork(
       error => Js.Log(error),
       response => Js.Log(response),
     );

let cancelFetch = fetchData();
```
