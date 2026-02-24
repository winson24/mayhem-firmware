# 🎙️ Friendly Walker – HackRF PortaPack External App

A Push-to-Talk (PTT) walkie-talkie app for the [HackRF PortaPack Mayhem firmware](https://github.com/portapack-mayhem/mayhem-firmware).  
It supports both screen-touch PTT and **physical headset button PTT** via the 3.5 mm TRRS jack.

---

## ✨ Features

| Feature | Details |
|---|---|
| **Headset PTT** | Uses the headset's inline button (GPIO1[6]) as a hardware Push-to-Talk trigger |
| **Headset detection** | Automatically detects when a headset mic is selected |
| **Band presets** | Quick buttons for 2m (145.000 MHz), 70cm (433.920 MHz), and PMR446 (446.006 MHz) |
| **Frequency tuning** | Full frequency field with adjustable step size (1 kHz – 25 kHz) |
| **RSSI meter** | Live signal strength display with colour coding (green / yellow / red) |
| **Narrowband FM** | TX and RX both use NB-FM (12.5 kHz) |
| **Debounced button** | 50 ms software debounce on the headset button |
| **Screen PTT** | On-screen PTT button as a fallback when no headset is connected |

---

## 📂 Project Structure

```
friendly_walker/
├── CMakeLists.txt                  # Build configuration (external app)
├── main.cpp                        # App entry point & app_information metadata
├── friendly_walker.hpp             # Class declaration / UI layout
├── friendly_walker_headset.cpp     # All logic: GPIO, audio, TX/RX switching
├── external-apps/                  # Pre-built .ppma files (optional – community apps)
└── README.md
```

---

## 🔧 How to Build

This app is built as part of the **Mayhem firmware** external-apps system.

### Requirements
- [Docker Desktop](https://www.docker.com/products/docker-desktop/) (recommended, cross-platform)
- OR a native ARM cross-compiler toolchain matching Mayhem's build system

### Steps

1. **Clone Mayhem firmware** (if you haven't already):
   ```bash
   git clone https://github.com/portapack-mayhem/mayhem-firmware.git
   cd mayhem-firmware
   git submodule update --init --recursive
   ```

2. **Copy the app into the firmware tree:**
   ```
   mayhem-firmware/
   └── firmware/
       └── application/
           └── apps/
               └── external/
                   └── friendly_walker/   ← paste your folder here
   ```

3. **Build with Docker:**
   ```bash
   docker run --rm -it \
     -v "$(pwd)":/mnt/mayhem \
     portapackmayhem/mayhem-toolchain \
     bash -c "cd /mnt/mayhem && mkdir -p build && cd build && cmake .. && make -j$(nproc) friendly_walker"
   ```

4. The compiled file will be at:
   ```
   build/firmware/application/apps/external/friendly_walker/friendly_walker.ppma
   ```

5. **Copy `friendly_walker.ppma` to your SD card:**
   ```
   SD Card/
   └── APPS/
       └── friendly_walker.ppma
   ```

6. Eject the SD card, insert into PortaPack, and navigate to **TX → Friendly Walker**.

---

## 🗺️ Hardware Notes

- The headset button GPIO is hardcoded to **GPIO1[6]** (the ring contact on the 3.5 mm TRRS jack).  
  If your board revision uses a different pin, edit this line in `friendly_walker_headset.cpp`:
  ```cpp
  static constexpr gpio::GPIO HEADSET_BTN_GPIO = gpio::GPIO{1, 6};
  ```
- **Transmitting is legal only on licensed frequencies** (amateur radio bands require a licence).  
  PMR446 (446 MHz) is licence-free in most of Europe at ≤ 0.5 W ERP.

---

## 📡 Default Frequencies

| Button | Frequency | Band |
|---|---|---|
| **2m** | 145.000 MHz | Amateur 2 m |
| **70cm** | 433.920 MHz | Amateur 70 cm |
| **PMR** | 446.006 MHz | PMR446 (licence-free) |

---

## ⚠️ Legal Disclaimer

This software is provided for **educational and research purposes only**.  
Transmitting on radio frequencies without the appropriate licence may be **illegal** in your country.  
Always comply with your local radio regulations (FCC, Ofcom, ACMA, etc.).

---

## 🤝 Contributing

Pull requests and issues are welcome! Please test on real hardware before submitting a PR.

---

## 📜 Licence

MIT – see [LICENSE](LICENSE) file for details.
