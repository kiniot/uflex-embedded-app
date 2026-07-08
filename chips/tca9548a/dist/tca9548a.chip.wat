;; Example WAT for the uFlex Wokwi TCA9548A custom chip.
;; This is a readable reference, not the compiled simulator artifact.
;; Wokwi loads `tca9548a.chip.wasm`, built from `chips/tca9548a/src/main.c`.
;;
;; Modeled behavior:
;; - fixed default address: 0x70
;; - upstream pins: SCL/SDA
;; - downstream pins: SC0/SD0 through SC7/SD7
;; - one control byte enables downstream channels by bitmask
;; - I2C reads return that current bitmask
;; - reset clears all enabled channels

(module
  (import "env" "memory" (memory 1))
  ;; pinInit(labelPtr, mode) returns an opaque pin handle. The label pointer
  ;; references one of the zero-terminated strings declared below with `data`.
  (import "env" "pinInit" (func $pinInit (param i32 i32) (result i32)))
  ;; pinMode(pinHandle, mode) changes how the simulated pin behaves after init.
  ;; The example uses this to connect/disconnect downstream mux channels.
  (import "env" "pinMode" (func $pinMode (param i32 i32)))
  ;; i2cInit would normally receive a Wokwi i2c_config_t pointer. In this
  ;; hand-written example, STATE is passed as a readable stand-in.
  (import "env" "i2cInit" (func $i2cInit (param i32) (result i32)))
  ;; trace(messagePtr) is only a teaching aid here. The C source uses printf.
  (import "env" "trace" (func $trace (param i32)))

  ;; These mode numbers are illustrative. Their purpose is to make the example
  ;; readable; the compiled C chip gets the real values from Wokwi headers.
  (global $INPUT i32 (i32.const 0))
  (global $OUTPUT i32 (i32.const 1))
  (global $INPUT_PULLUP i32 (i32.const 2))
  (global $ADDRESS i32 (i32.const 0x70))

  ;; Fixed state block:
  ;; +0 channel_enable byte
  ;; +4 RESET pin
  ;; +8 upstream SCL pin
  ;; +12 upstream SDA pin
  ;; +16 eight downstream SC/SD pin pairs, 8 bytes per channel
  (global $STATE i32 (i32.const 1024))
  (global $CHANNEL_ENABLE i32 (i32.const 0))
  (global $RESET_PIN i32 (i32.const 4))
  (global $UPSTREAM_SCL_PIN i32 (i32.const 8))
  (global $UPSTREAM_SDA_PIN i32 (i32.const 12))
  (global $CHANNEL_PINS i32 (i32.const 16))
  (global $CHANNEL_STRIDE i32 (i32.const 8))

  ;; WAT data segments place strings directly into linear memory. The offsets
  ;; are arbitrary but fixed, so functions can pass them to pinInit/trace.
  (data (i32.const 64) "RESET\00")
  (data (i32.const 80) "SCL\00")
  (data (i32.const 96) "SDA\00")
  (data (i32.const 112) "SC0\00")
  (data (i32.const 128) "SD0\00")
  (data (i32.const 144) "SC1\00")
  (data (i32.const 160) "SD1\00")
  (data (i32.const 176) "SC2\00")
  (data (i32.const 192) "SD2\00")
  (data (i32.const 208) "SC3\00")
  (data (i32.const 224) "SD3\00")
  (data (i32.const 240) "SC4\00")
  (data (i32.const 256) "SD4\00")
  (data (i32.const 272) "SC5\00")
  (data (i32.const 288) "SD5\00")
  (data (i32.const 304) "SC6\00")
  (data (i32.const 320) "SD6\00")
  (data (i32.const 336) "SC7\00")
  (data (i32.const 352) "SD7\00")
  (data (i32.const 384) "chip_init\00")

  ;; Wokwi probes this symbol to confirm the custom-chip ABI version.
  (func $__wokwi_api_version_1 (result i32)
    i32.const 1)

  ;; Convert a state-block offset into an absolute linear-memory address.
  ;; Example: stateAddr(4) points to the RESET pin handle slot.
  (func $stateAddr (param $offset i32) (result i32)
    global.get $STATE
    local.get $offset
    i32.add)

  ;; Each channel stores two i32 pin handles: SCn then SDn. The base address is
  ;; STATE + CHANNEL_PINS + channel * CHANNEL_STRIDE.
  (func $channelBase (param $channel i32) (result i32)
    global.get $STATE
    global.get $CHANNEL_PINS
    i32.add
    local.get $channel
    global.get $CHANNEL_STRIDE
    i32.mul
    i32.add)

  ;; The labels are laid out as SC0, SD0, SC1, SD1... every 16 bytes. This lets
  ;; the example avoid string formatting while still initializing all channels.
  (func $sclLabel (param $channel i32) (result i32)
    i32.const 112
    local.get $channel
    i32.const 16
    i32.mul
    i32.add)

  ;; SDA labels start 16 bytes after the matching SCL labels.
  (func $sdaLabel (param $channel i32) (result i32)
    i32.const 128
    local.get $channel
    i32.const 16
    i32.mul
    i32.add)

  ;; The TCA9548A control register is one byte wide. store8 intentionally keeps
  ;; only the low 8 bits even though WAT arithmetic values are i32.
  (func $setMask (param $mask i32)
    global.get $CHANNEL_ENABLE
    call $stateAddr
    local.get $mask
    i32.store8)

  ;; load8_u reads the byte back as an unsigned i32, matching an I2C register
  ;; read where values are returned as 0..255.
  (func $getMask (result i32)
    global.get $CHANNEL_ENABLE
    call $stateAddr
    i32.load8_u)

  ;; Store the two downstream pin handles for a channel. In the real C source
  ;; this is represented by pins_scl_sda[channel][0 or 1].
  (func $storeChannel (param $channel i32) (param $scl i32) (param $sda i32)
    local.get $channel
    call $channelBase
    local.get $scl
    i32.store
    local.get $channel
    call $channelBase
    i32.const 4
    i32.add
    local.get $sda
    i32.store)

  ;; Load the SCn handle from the first slot of the channel pair.
  (func $loadScl (param $channel i32) (result i32)
    local.get $channel
    call $channelBase
    i32.load)

  ;; Load the SDn handle from the second slot of the channel pair.
  (func $loadSda (param $channel i32) (result i32)
    local.get $channel
    call $channelBase
    i32.const 4
    i32.add
    i32.load)

  ;; Initialize RESET and the upstream I2C bus pins. They use pull-ups because
  ;; I2C lines idle high and the reset input should remain inactive by default.
  (func $initControlPins
    i32.const 64
    global.get $INPUT_PULLUP
    call $pinInit
    global.get $RESET_PIN
    call $stateAddr
    i32.store
    i32.const 80
    global.get $INPUT_PULLUP
    call $pinInit
    global.get $UPSTREAM_SCL_PIN
    call $stateAddr
    i32.store
    i32.const 96
    global.get $INPUT_PULLUP
    call $pinInit
    global.get $UPSTREAM_SDA_PIN
    call $stateAddr
    i32.store)

  ;; Initialize one downstream channel. Channels start as INPUT, which models a
  ;; disconnected downstream bus until the control register enables that bit.
  (func $initChannel (param $channel i32)
    (local $scl i32)
    (local $sda i32)
    local.get $channel
    call $sclLabel
    global.get $INPUT
    call $pinInit
    local.set $scl
    local.get $channel
    call $sdaLabel
    global.get $INPUT
    call $pinInit
    local.set $sda
    local.get $channel
    local.get $scl
    local.get $sda
    call $storeChannel)

  ;; WAT has structured loops rather than C-style for loops. This loop calls
  ;; initChannel(0) through initChannel(7), then exits when channel == 8.
  (func $initChannels
    (local $channel i32)
    i32.const 0
    local.set $channel
    (loop $next
      local.get $channel
      call $initChannel
      local.get $channel
      i32.const 1
      i32.add
      local.tee $channel
      i32.const 8
      i32.lt_u
      br_if $next))

  ;; Apply one bit of the control mask:
  ;; - if mask & (1 << channel) is nonzero, downstream SCn/SDn become OUTPUT
  ;; - otherwise they become INPUT, representing a disconnected branch
  (func $applyChannel (param $channel i32) (param $mask i32)
    (local $mode i32)
    local.get $mask
    i32.const 1
    local.get $channel
    i32.shl
    i32.and
    (if
      (then global.get $OUTPUT local.set $mode)
      (else global.get $INPUT local.set $mode))
    local.get $channel
    call $loadScl
    local.get $mode
    call $pinMode
    local.get $channel
    call $loadSda
    local.get $mode
    call $pinMode)

  ;; Re-apply the current mask to all eight channels. The real chip changes the
  ;; switch matrix atomically; this loop is a simple simulator-friendly model.
  (func $applyMask (param $mask i32)
    (local $channel i32)
    i32.const 0
    local.set $channel
    (loop $next
      local.get $channel
      local.get $mask
      call $applyChannel
      local.get $channel
      i32.const 1
      i32.add
      local.tee $channel
      i32.const 8
      i32.lt_u
      br_if $next))

  ;; Reset state to power-on behavior: no downstream channel selected.
  (func $resetChannels
    i32.const 0
    call $setMask
    i32.const 0
    call $applyMask)

  ;; Return true only when the master addresses this mux. `$isRead` is accepted
  ;; so the signature resembles an I2C connect callback, even though this simple
  ;; address check does not need it.
  (func $onI2cConnect (param $address i32) (param $isRead i32) (result i32)
    local.get $address
    global.get $ADDRESS
    i32.eq)

  ;; A TCA9548A read returns the control register, so firmware can confirm which
  ;; channels are currently enabled.
  (func $onI2cRead (result i32)
    call $getMask)

  ;; A TCA9548A write replaces the control register. For example:
  ;; - 0x01 selects channel 0
  ;; - 0x04 selects channel 2
  ;; - 0x05 selects channels 0 and 2 together
  (func $onI2cWrite (param $data i32) (result i32)
    (local $mask i32)
    local.get $data
    i32.const 0xff
    i32.and
    local.tee $mask
    call $setMask
    local.get $mask
    call $applyMask
    i32.const 1)

  ;; No per-transaction cleanup is needed for this simplified model. The enabled
  ;; channel mask persists until the next write or reset, like the real part.
  (func $onI2cDisconnect)

  ;; Main chip entry point. The C version allocates chip_state_t and registers
  ;; callback function pointers; this WAT version uses fixed memory for clarity.
  (func $chipInit
    i32.const 384
    call $trace
    call $initControlPins
    call $initChannels
    call $resetChannels
    ;; The C SDK receives an i2c_config_t with callbacks. Here STATE is a stand-in.
    global.get $STATE
    call $i2cInit
    drop)

  (export "__wokwi_api_version_1" (func $__wokwi_api_version_1))
  (export "chipInit" (func $chipInit))
  (export "onI2cConnect" (func $onI2cConnect))
  (export "onI2cRead" (func $onI2cRead))
  (export "onI2cWrite" (func $onI2cWrite))
  (export "onI2cDisconnect" (func $onI2cDisconnect))
)
