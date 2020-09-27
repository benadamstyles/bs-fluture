open Jest;
open BsFluture;
open Expect;
open! Operators;
open Belt;

let state = ref(false);

beforeEach(() => state := false);

let future =
  make((_rej, res) => {
    let tid =
      Js.Global.setTimeout(
        () => {
          state := true;
          res(true);
        },
        30,
      );
    Cancel(() => Js.Global.clearTimeout(tid));
  });

let failingFuture =
  make((rej, _res) => {
    let tid = Js.Global.setTimeout(() => rej(false), 30);
    Cancel(() => Js.Global.clearTimeout(tid));
  });

let futureWithNoCancellation =
  make((_rej, res) => {
    Js.Global.setTimeout(
      () => {
        state := true;
        res(true);
      },
      30,
    )
    |> ignore;
    NoCancel;
  });

let throwOnCall = () => Js.Exn.raiseError("Error thrown unexpectedly");
let throwOnCallError = (_error: Js.Exn.t) => throwOnCall();
let throwOnCallString = (_error: string) => throwOnCall();
let throwOnCallBool = (_error: bool) => throwOnCall();

describe("BsFluture", () => {
  describe("cancellation", () => {
    testAsync("not cancelling works as expected", test => {
      let succeeded = ref(false);

      future |> fork(throwOnCall, _res => succeeded := true) |> ignore;

      Js.Global.setTimeout(
        () => test(expect((state^, succeeded^)) == (true, true)),
        90,
      )
      |> ignore;
    });

    testAsync(
      "with cancellation, cancelling prevents fork handlers and computation",
      test => {
      let succeeded = ref(false);

      let cancel = future |> fork(throwOnCall, _res => succeeded := true);

      cancel(.);

      Js.Global.setTimeout(
        () => test(expect((state^, succeeded^)) == (false, false)),
        90,
      )
      |> ignore;
    });

    testAsync(
      "with no cancellation, cancelling prevents fork handlers but not computation",
      test => {
        let succeeded = ref(false);

        let cancel =
          futureWithNoCancellation
          |> fork(throwOnCall, _res => succeeded := true);

        cancel(.);

        Js.Global.setTimeout(
          () => test(expect((state^, succeeded^)) == (true, false)),
          90,
        )
        |> ignore;
      },
    );
  });

  describe("static functions", () => {
    describe("resolve", () =>
      testAsync("forks to a success value", test => {
        resolve(7)
        |> fork(throwOnCall, res => test(expect(res) === 7))
        |> ignore
      })
    );

    describe("reject", () =>
      testAsync("forks to a failure value", test => {
        reject(9)
        |> fork(res => test(expect(res) === 9), throwOnCall)
        |> ignore
      })
    );

    describe("after", () =>
      testAsync("forks to a success value", test => {
        after(10, "later")
        |> fork(throwOnCall, res => test(expect(res) === "later"))
        |> ignore
      })
    );

    describe("rejectAfter", () =>
      testAsync("forks to a failure value", test => {
        rejectAfter(10, "later")
        |> fork(res => test(expect(res) === "later"), throwOnCall)
        |> ignore
      })
    );

    describe("attempt", () => {
      testAsync("forks to a success value when not throwing", test => {
        attempt(() => "success")
        |> fork(throwOnCallError, res => test(expect(res) === "success"))
        |> ignore
      });

      testAsync("forks to a failure value when throwing", test => {
        attempt(() => Js.Exn.raiseError("failure"))
        |> fork(
             res =>
               test(
                 expect(res->Js.Exn.message->Option.getExn) === "failure",
               ),
             throwOnCall,
           )
        |> ignore
      });
    });

    describe("attemptP", () => {
      testAsync("forks to a success value when resolving", test => {
        attemptP(() => Js.Promise.resolve("success"))
        |> fork(throwOnCall, res => test(expect(res) === "success"))
        |> ignore
      });

      testAsync("forks to a failure value when rejecting", test => {
        attemptP(() => Js.Promise.reject(Not_found))
        |> fork(res => test(expect(res) == Not_found), throwOnCall)
        |> ignore
      });

      testAsync("forks to a failure value when throwing", test => {
        attemptP(() =>
          Js.Promise.make((~resolve as _, ~reject as _) =>
            Js.Exn.raiseError("failure")
          )
        )
        |> fork(
             res =>
               test(
                 expect(res->Js.Exn.message->Option.getExn) === "failure",
               ),
             throwOnCall,
           )
        |> ignore
      });
    });

    describe("node", () => {
      testAsync("forks to a success value when passing a value", test => {
        node(callback => callback(Js.null, Some("success")))
        |> fork(throwOnCall, res => test(expect(res) === "success"))
        |> ignore
      });

      testAsync("forks to a failure value when passing an error", test => {
        node(callback =>
          callback(Js.Null.return(Not_found), Some("success"))
        )
        |> fork(res => test(expect(res) == Not_found), throwOnCallString)
        |> ignore
      });
    });

    describe("encase", () => {
      testAsync("forks to a success value when returning a value", test => {
        encase(value => value * 2, 7)
        |> fork(throwOnCallError, res => test(expect(res) === 14))
        |> ignore
      });

      testAsync("forks to a failure value when throwing", test => {
        encase(() => Js.Exn.raiseError("failure"), ())
        |> fork(
             res =>
               test(
                 expect(res->Js.Exn.message->Option.getExn) === "failure",
               ),
             throwOnCallString,
           )
        |> ignore
      });
    });

    describe("encaseP", () => {
      testAsync("forks to a success value when resolving", test => {
        encaseP(value => Js.Promise.resolve(value), "success")
        |> fork(throwOnCallError, res => test(expect(res) === "success"))
        |> ignore
      });

      testAsync("forks to a failure value when rejecting", test => {
        encaseP(_ => Js.Promise.reject(Not_found), "success")
        |> fork(res => test(expect(res) == Not_found), throwOnCall)
        |> ignore
      });

      testAsync("forks to a failure value when throwing", test => {
        encaseP(
          _ =>
            Js.Promise.make((~resolve as _, ~reject as _) =>
              Js.Exn.raiseError("failure")
            ),
          "success",
        )
        |> fork(
             res =>
               test(
                 expect(res->Js.Exn.message->Option.getExn) === "failure",
               ),
             throwOnCall,
           )
        |> ignore
      });
    });

    describe("map", () => {
      testAsync("transforms the success value", test => {
        future
        |> map(bool => string_of_bool(bool) ++ "!")
        |> fork(throwOnCall, res => test(expect(res) === "true!"))
        |> ignore
      });

      testAsync("does not transform the failure value", test => {
        reject(false)
        |> map(bool => string_of_bool(bool) ++ "!")
        |> fork(res => test(expect(res) === false), throwOnCallString)
        |> ignore
      });
    });

    describe("mapRej", () => {
      testAsync("does not transform the success value", test => {
        resolve(true)
        |> mapRej(bool => string_of_bool(bool) ++ "!")
        |> fork(throwOnCallString, res => test(expect(res) === true))
        |> ignore
      });

      testAsync("transforms the failure value", test => {
        reject(false)
        |> mapRej(bool => string_of_bool(bool) ++ "!")
        |> fork(res => test(expect(res) === "false!"), throwOnCallString)
        |> ignore
      });
    });

    describe("bimap", () => {
      let mapper = bool => string_of_bool(bool) ++ "!";

      testAsync("transforms the success value", test => {
        resolve(true)
        |> bimap(mapper, mapper)
        |> fork(throwOnCallString, res => test(expect(res) === "true!"))
        |> ignore
      });

      testAsync("transforms the failure value", test => {
        reject(false)
        |> bimap(mapper, mapper)
        |> fork(res => test(expect(res) === "false!"), throwOnCallString)
        |> ignore
      });
    });

    describe("chain", () => {
      testAsync("transforms the success value", test => {
        future
        |> chain(bool => resolve(string_of_bool(bool) ++ "!"))
        |> fork(throwOnCall, res => test(expect(res) === "true!"))
        |> ignore
      });

      testAsync(
        "rejects on transforming the success value to a rejected future", test => {
        resolve(true)
        |> chain(bool => reject(string_of_bool(!bool) ++ "!"))
        |> fork(res => test(expect(res) === "false!"), throwOnCallBool)
        |> ignore
      });

      testAsync("does not transform the failure value", test => {
        reject(false)
        |> chain(bool => resolve(string_of_bool(bool) ++ "!"))
        |> fork(res => test(expect(res) === false), throwOnCallString)
        |> ignore
      });
    });

    describe("chainRej", () => {
      testAsync("does not transform the success value", test => {
        resolve(true)
        |> chainRej(bool => reject(string_of_bool(bool) ++ "!"))
        |> fork(throwOnCallString, res => test(expect(res) === true))
        |> ignore
      });

      testAsync(
        "resolves on transforming the failure value to a resolved future", test => {
        reject(false)
        |> chainRej(bool => resolve(string_of_bool(!bool) ++ "!"))
        |> fork(throwOnCallBool, res => test(expect(res) === "true!"))
        |> ignore
      });

      testAsync("transforms the failure value", test => {
        reject(false)
        |> chainRej(bool => reject(string_of_bool(bool) ++ "!"))
        |> fork(res => test(expect(res) === "false!"), throwOnCallString)
        |> ignore
      });
    });

    describe("bichain", () => {
      let mapToRes = bool => resolve(string_of_bool(bool) ++ "!");
      let mapToRej = bool => reject(string_of_bool(!bool) ++ "!");

      testAsync("transforms the success value", test => {
        resolve(true)
        |> bichain(mapToRej, mapToRes)
        |> fork(throwOnCallString, res => test(expect(res) === "true!"))
        |> ignore
      });

      testAsync("transforms the success to a failure", test => {
        resolve(true)
        |> bichain(mapToRej, mapToRej)
        |> fork(res => test(expect(res) === "false!"), throwOnCallBool)
        |> ignore
      });

      testAsync("transforms the failure value", test => {
        reject(false)
        |> bichain(mapToRej, mapToRes)
        |> fork(res => test(expect(res) === "true!"), throwOnCallString)
        |> ignore
      });

      testAsync("transforms the failure to a success", test => {
        reject(false)
        |> bichain(mapToRes, mapToRes)
        |> fork(throwOnCallString, res => test(expect(res) === "false!"))
        |> ignore
      });
    });

    describe("swap", () => {
      testAsync("successfully swaps the success and failure branches", test => {
        resolve(true)
        |> swap
        |> fork(res => test(expect(res) === true), throwOnCall)
        |> ignore
      })
    });

    describe("coalesce", () => {
      let mapper = bool => string_of_bool(bool) ++ "!";

      testAsync("successfully coalesces the success branch", test => {
        resolve(true)
        |> coalesce(mapper, mapper)
        |> fork(throwOnCall, res => test(expect(res) === "true!"))
        |> ignore
      });

      testAsync("successfully coalesces the failure branch", test => {
        reject(false)
        |> coalesce(mapper, mapper)
        |> fork(throwOnCall, res => test(expect(res) === "false!"))
        |> ignore
      });
    });

    describe("fork", () => {
      testAsync("success branch", test =>
        future
        |> fork(throwOnCall, res => test(expect(res) === true))
        |> ignore
      );

      testAsync("failure branch", test =>
        reject(false)
        |> fork(res => test(expect(res) === false), throwOnCall)
        |> ignore
      );
    });

    describe("forkCatch", () => {
      testAsync("success branch", test =>
        future
        |> fork(throwOnCall, res => test(expect(res) === true))
        |> ignore
      );

      testAsync("failure branch", test =>
        reject(false)
        |> fork(res => test(expect(res) === false), throwOnCall)
        |> ignore
      );

      testAsync("thrown errors are caught", test =>
        resolve(true)
        |> map(_ => Js.Exn.raiseError("exception"))
        |> forkCatch(
             ex =>
               test(
                 expect(ex->Js.Exn.message->Option.getExn) === "exception",
               ),
             throwOnCall,
             throwOnCall,
           )
        |> ignore
      );
    });

    describe("value", () => {
      testAsync("is called with the success value", test =>
        resolve(3) |> value(res => test(expect(res) === 3)) |> ignore
      )
    });

    describe("done_", () => {
      testAsync("success branch", test =>
        future
        |> done_((err, res) =>
             test(expect((err, res)) == (Js.null, Some(true)))
           )
        |> ignore
      );

      testAsync("failure branch", test =>
        reject(false)
        |> done_((err, res) =>
             test(expect((err, res)) == (Js.Null.return(false), None))
           )
        |> ignore
      );
    });

    describe("promise", () => {
      testAsync("success branch", test =>
        Js.(
          future
          |> promise
          |> Promise.catch(_err => throwOnCall())
          |> Promise.then_(value =>
               test(expect(value) === true)->Promise.resolve
             )
          |> ignore
        )
      );

      testAsync("failure branch", test =>
        reject(false)
        |> promise
        |> Js.Promise.then_(throwOnCall)
        |> Js.Promise.catch(err =>
             test(expect(err->Obj.magic) === false)->Js.Promise.resolve
           )
        |> ignore
      );
    });

    describe("race", () => {
      testAsync("success branch", test =>
        race(after(10, true), after(15, false))
        |> value(value => test(expect(value) === true))
        |> ignore
      );

      testAsync("failure branch", test =>
        race(failingFuture, after(35, false))
        |> fork(value => test(expect(value) === false), throwOnCallBool)
        |> ignore
      );
    });

    describe("both", () => {
      testAsync("success branch", test =>
        both(after(10, true), after(15, false))
        |> value(((after10, after15)) =>
             test(expect((after10, after15)) == (true, false))
           )
        |> ignore
      );

      testAsync("failure branch", test =>
        both(failingFuture, after(35, false))
        |> fork(value => test(expect(value) === false), _ => throwOnCall())
        |> ignore
      );
    });

    describe("parallel", () => {
      testAsync("success branch", test =>
        parallel(2, [|after(10, 1), after(15, 2), after(20, 3)|])
        |> value(arr => test(expect(arr) == [|1, 2, 3|]))
        |> ignore
      );

      testAsync("failure branch", test =>
        parallel(2, [|failingFuture, after(5, true), after(35, false)|])
        |> fork(value => test(expect(value) === false), _ => throwOnCall())
        |> ignore
      );
    });

    describe("isFuture", () => {
      test("returns true for a future", () =>
        expect(future->isFuture) === true
      );

      test("returns true for a never", () =>
        expect(never->isFuture) === true
      );

      test("returns false for a Promise", () =>
        expect(Js.Promise.resolve(true)->isFuture) === false
      );
    });

    describe("never", () => {
      testAsync("does not settle", test => {
        never |> fork(throwOnCall, throwOnCall) |> ignore;
        Js.Global.setTimeout(() => test(expect(true) === true), 50)
        |> ignore;
      })
    });

    describe("isNever", () => {
      test("returns true for a never", () =>
        expect(never->isNever) === true
      );

      test("returns false for another future", () =>
        expect(future->isNever) === false
      );
    });
  });
});
