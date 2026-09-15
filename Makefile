ARDUINO_CLI ?= arduino-cli
CONFIG_DIR := .arduino-cli
PROJECT_HOME := $(CURDIR)/.home
ESP32_INDEX_URL := https://espressif.github.io/arduino-esp32/package_esp32_index.json
FQBN := esp32:esp32:XIAO_ESP32C6
HOMEKIT_BOARD_OPTIONS := PartitionScheme=huge_app
PORT ?= /dev/cu.usbmodem101

SERIAL_SKETCH := firmware/SerialTest
SERIAL_BUILD := $(CURDIR)/.arduino-cli/build/SerialTest
SERIAL_OUTPUT := $(CURDIR)/.arduino-cli/output/SerialTest
RELAY_SKETCH := firmware/RelayPulse
RELAY_BUILD := $(CURDIR)/.arduino-cli/build/RelayPulse
RELAY_OUTPUT := $(CURDIR)/.arduino-cli/output/RelayPulse
HOMEKIT_SKETCH := firmware/HomeKitDoorRelease
HOMEKIT_PROD_BUILD := $(CURDIR)/.arduino-cli/build/HomeKitDoorRelease-prod
HOMEKIT_PROD_OUTPUT := $(CURDIR)/.arduino-cli/output/HomeKitDoorRelease-prod
HOMEKIT_DEBUG_BUILD := $(CURDIR)/.arduino-cli/build/HomeKitDoorRelease-debug
HOMEKIT_DEBUG_OUTPUT := $(CURDIR)/.arduino-cli/output/HomeKitDoorRelease-debug
HOMEKIT_PROD_FLAGS := -DSERIAL_DIAGNOSTICS=0 -DSERIAL_PORT_ENABLED=1
HOMEKIT_DEBUG_FLAGS := -DSERIAL_DIAGNOSTICS=1 -DSERIAL_PORT_ENABLED=1

export HOME := $(PROJECT_HOME)

.PHONY: setup-cli version core-list board-list board-listall compile-serial upload-serial compile-relay upload-relay compile-homekit compile-homekit-prod compile-homekit-debug upload-homekit upload-homekit-prod upload-homekit-debug monitor

setup-cli:
	mkdir -p $(CONFIG_DIR)
	$(ARDUINO_CLI) config init --overwrite --dest-dir $(CONFIG_DIR)
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) config set board_manager.additional_urls $(ESP32_INDEX_URL)
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) config set directories.data $(CURDIR)/.arduino-cli/data
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) config set directories.downloads $(CURDIR)/.arduino-cli/downloads
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) config set directories.user $(CURDIR)/sketchbook
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) core update-index
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) core install esp32:esp32
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) lib install HomeSpan

version:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) version

core-list:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) core list

board-list:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) board list

board-listall:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) board listall xiao

compile-serial:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) compile --fqbn $(FQBN) --build-path $(SERIAL_BUILD) --output-dir $(SERIAL_OUTPUT) $(SERIAL_SKETCH)

upload-serial:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) upload -p $(PORT) --fqbn $(FQBN) --build-path $(SERIAL_BUILD) $(SERIAL_SKETCH)

compile-relay:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) compile --fqbn $(FQBN) --build-path $(RELAY_BUILD) --output-dir $(RELAY_OUTPUT) $(RELAY_SKETCH)

upload-relay:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) upload -p $(PORT) --fqbn $(FQBN) --build-path $(RELAY_BUILD) $(RELAY_SKETCH)

compile-homekit:
	$(MAKE) compile-homekit-prod

compile-homekit-prod:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) compile --fqbn $(FQBN) --board-options $(HOMEKIT_BOARD_OPTIONS) --build-property compiler.cpp.extra_flags="$(HOMEKIT_PROD_FLAGS)" --build-path $(HOMEKIT_PROD_BUILD) --output-dir $(HOMEKIT_PROD_OUTPUT) $(HOMEKIT_SKETCH)

compile-homekit-debug:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) compile --fqbn $(FQBN) --board-options $(HOMEKIT_BOARD_OPTIONS) --build-property compiler.cpp.extra_flags="$(HOMEKIT_DEBUG_FLAGS)" --build-path $(HOMEKIT_DEBUG_BUILD) --output-dir $(HOMEKIT_DEBUG_OUTPUT) $(HOMEKIT_SKETCH)

upload-homekit:
	$(MAKE) upload-homekit-prod

upload-homekit-prod: compile-homekit-prod
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) upload -p $(PORT) --fqbn $(FQBN) --board-options $(HOMEKIT_BOARD_OPTIONS) --build-path $(HOMEKIT_PROD_BUILD) $(HOMEKIT_SKETCH)

upload-homekit-debug: compile-homekit-debug
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) upload -p $(PORT) --fqbn $(FQBN) --board-options $(HOMEKIT_BOARD_OPTIONS) --build-path $(HOMEKIT_DEBUG_BUILD) $(HOMEKIT_SKETCH)

monitor:
	$(ARDUINO_CLI) --config-dir $(CONFIG_DIR) monitor -p $(PORT) -c baudrate=115200
