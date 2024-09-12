# zouipocar embedded

This is the program that is flashed on the embedded part of zouipocar. It was developed with an Arduino Uno Rev3 so it targets the ATmega328P processor communicating with a Quectel MC60 GPS/GPRS module.

## Parts

Parts used :

- [Arduino Uno Rev3](https://store.arduino.cc/en-fr/products/arduino-uno-rev3?srsltid=AfmBOooGgRJIDZPFExjhLVOX6uHIUJTTCBOTn38zgc6NyL2movwVAxmV)
- [Quectel MC60 GPS/GPRS module](https://www.quectel.com/product/gsm-gprs-gnss-mc60/)

These parts are optional, depending on how the thing is set up :

- [MC60 shield for Arduino Uno](https://itbrainpower.net/arduino-gsm-gps-shield-dual-sim-integrated-antenna-USB-SD-bluetooth_b-gsmgnss/resources.php)
- [5.5-2.1 barrel jack cable](https://www.conrad.fr/fr/p/cable-de-raccordement-basse-tension-tru-components-tc-2511268-dc-male-extremite-s-ouverte-s-5-50-mm-2-10-mm-0-50-m-1715079.html)
- [Fuse tap](https://www.amazon.fr/dp/B07MF3V9K1?psc=1&ref=ppx_yo2ov_dt_b_product_details) to power the device from a car battery
- [GPS antenna](https://sixfab.com/product/passive-gps-antenna-4db-peak-gain-u-fl-plug/)
- [LTE antenna](https://sixfab.com/product/lte-5g-sma-high-performance-blade-antenna/)
- [SMA to u.FL RF Adapter Cable](https://sixfab.com/product/sma-to-u-fl-rf-adapter-cable-150mm/)

The shield can be directly plugged onto an Arduino Uno. The following pins must be wired :

- D2 (MC60's TX) <=> D0 (Uno's UART RX)
- D3 (MC60's RX) <=> D1 (Uno's UART TX)

## Power

The Uno with the shield sitting on it can drain quite a lot of current in short bursts, mainly due to the LTE communication. For that reason it is not possible to power it via the USB port.

The device must be powered either via the barrel jack or the VIN pin with a 12V power supply (6-20V tolerated). At least 2-3 amps are required.

## Debug

During development the program was debugged with a logic analyzer such as [this one](https://www.amazon.fr/AZDelivery-Logic-Analyser-compatible-version/dp/B01MUFRHQ2?th=1).

During debug the Uno and shield pins must be linked as described above. Additionally the following connections must be made :

- D7 (Uno) <=> D7 (MC60 PWRKEY pin), otherwise the MC60 doesn't turn on
- GND (Logic Analyzer) <=> GND (Uno)
- Any input (Logic Analyzer) <=> D0 (Uno)
- Another input (Logic Analyzer) <=> D1 (Uno)

The logic analyzer can then most likely be used with the [Salae Logic](https://support.saleae.com/logic-software) software ([AUR package](https://aur.archlinux.org/packages/saleae-logic2)).

## Flash

    make flash