# [2.0.0](https://github.com/benadamstyles/bs-fluture/compare/v1.0.0...v2.0.0) (2020-10-03)


### Bug Fixes

* **bsconfig:** remove namespacing ([432ed55](https://github.com/benadamstyles/bs-fluture/commit/432ed55b2048b172fde6ab407d0cfbe862fa7aad))


### Code Refactoring

* **core:** change the module's namespace from BsFluture to Fluture ([ad2cd8a](https://github.com/benadamstyles/bs-fluture/commit/ad2cd8a270440092e6fa17cfed088b8578b4f873))


### BREAKING CHANGES

* **core:** You will need to update your code to reference the `Fluture` module rather than `BsFluture`.

# 1.0.0 (2020-09-27)


### Code Refactoring

* **core:** upgrade bindings to support Fluture v12+ ([fb41957](https://github.com/benadamstyles/bs-fluture/commit/fb41957))


### BREAKING CHANGES

* **core:** The bindings have been updated to support Fluture v12+, which introduced major breaking changes to the Fluture API as detailed [here](https://gist.github.com/Avaq/ee2c6c819db4c37258e9a226e6380a38#functions-use-simple-currying).