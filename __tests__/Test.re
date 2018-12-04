open Jest;
open BsFluture;
open Expect;

describe("BsFluture", () => {
  let future =
    make((_rej, res) => {
      let tid = Js.Global.setTimeout(() => res(true), 30);
      Cancel(() => Js.Global.clearTimeout(tid));
    });

  testAsync("fork success", test =>
    future |> fork(Js.log, res => test(expect(res) |> toBe(true))) |> ignore
  );

  testAsync("cancellation", test => {
    let succeeded = ref(false);

    let cancel = future |> fork(Js.log, _res => succeeded := true);

    safeCancel(cancel);

    Js.Global.setTimeout(() => test(expect(succeeded^) |> toBe(false)), 90)
    |> ignore;
  });
});
