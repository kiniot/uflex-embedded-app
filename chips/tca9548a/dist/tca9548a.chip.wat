;; Example WAT for a Wokwi custom chip.
;; This is a readable reference file, not the compiled artifact used by the simulator.
;; The simulator still loads `tca9548a.chip.wasm`.

(module
  ;; Wokwi provides linear memory from the host.
  (import "env" "memory" (memory 1))

  ;; Typical custom-chip host APIs.
  (import "env" "pinInit" (func $pinInit (param i32 i32) (result i32)))
  (import "env" "i2cInit" (func $i2cInit (param i32) (result i32)))
  (import "env" "pinMode" (func $pinMode (param i32 i32)))

  ;; Wokwi calls this to verify the ABI version.
  (func $__wokwi_api_version_1 (result i32)
    i32.const 1)

  ;; Minimal placeholder chip init.
  ;; A real implementation would allocate state, initialize pins, and register I2C callbacks.
  (func $chipInit)

  (export "__wokwi_api_version_1" (func $__wokwi_api_version_1))
  (export "chipInit" (func $chipInit))
)
