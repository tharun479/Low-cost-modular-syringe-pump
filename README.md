# 💉 Low Cost Modular Syringe Pump

<p align="center">
  <img src="https://img.shields.io/badge/Domain-Embedded%20Systems-blue?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Platform-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white" />
  <img src="https://img.shields.io/badge/Driver-A4988-orange?style=for-the-badge" />
  <img src="https://img.shields.io/badge/Simulation-Wokwi-purple?style=for-the-badge" />
</p>

<p align="center">
  <b>A low-cost, modular syringe pump for controlled and repeatable fluid delivery.</b>
</p>

<p align="center">
  <a href="https://wokwi.com/projects/446316151195038721">
    <img src="https://img.shields.io/badge/▶%20Run%20Wokwi%20Simulation-Click%20Here-success?style=for-the-badge" />
  </a>
</p>

---

## 📌 Overview

The **Low Cost Modular Syringe Pump** is an embedded-system prototype designed for controlled and repeatable fluid delivery using a **stepper motor and microcontroller-based control system**.

The system controls the **linear displacement of a syringe plunger** through a stepper motor. User inputs are provided through a keypad and the selected parameters are displayed on an LCD.

The project combines:

* 🧠 **Arduino microcontroller**
* ⚙️ **A4988 stepper motor driver**
* 🔄 **NEMA 17 stepper motor**
* ⌨️ **4×4 keypad**
* 📟 **16×2 I2C LCD**
* 💉 **Syringe-based fluid delivery mechanism**

---

## ✨ Key Features

| Feature                    | Description                                                           |
| -------------------------- | --------------------------------------------------------------------- |
| 💧 **Controlled Delivery** | Controls syringe plunger movement for fluid delivery                  |
| ⚙️ **Stepper Control**     | Precise motor movement using STEP/DIR signals                         |
| ⌨️ **User Input**          | Parameters entered through a 4×4 keypad                               |
| 📟 **LCD Interface**       | Displays operating parameters and system information                  |
| 🔧 **Modular Design**      | Components can be independently replaced or modified                  |
| 💰 **Low Cost**            | Uses readily available electronic components                          |
| 🧪 **Simulation**          | Arduino logic and peripherals tested using Wokwi                      |
| 🔬 **Prototype Testing**   | Hardware operation validated through subsystem and integrated testing |

---

## 🏗️ System Architecture

```text
                 ┌─────────────────────┐
                 │       4×4 Keypad    │
                 │    User Interface   │
                 └──────────┬──────────┘
                            │
                            ▼
                 ┌─────────────────────┐
                 │       ARDUINO       │
                 │   Main Controller   │
                 └─────────┬───┬───────┘
                           │   │
                     STEP/DIR  │ I²C
                           │   │
                           ▼   ▼
                    ┌────────┐ ┌──────────┐
                    │ A4988  │ │ 16×2 LCD │
                    │ Driver │ │ Display  │
                    └───┬────┘ └──────────┘
                        │
                        ▼
                  ┌─────────────┐
                  │   NEMA 17   │
                  │Stepper Motor│
                  └──────┬──────┘
                         │
                         ▼
                ┌──────────────────┐
                │ Mechanical Drive │
                │ Plunger Movement │
                └────────┬─────────┘
                         │
                         ▼
                ┌──────────────────┐
                │     SYRINGE      │
                │ Fluid Delivery   │
                └──────────────────┘
```

---

## 🔩 Hardware Components

| Component                | Purpose                                           |
| :----------------------- | :------------------------------------------------ |
| 🧠 **Arduino**           | Main system controller                            |
| ⚙️ **A4988**             | Drives the NEMA 17 stepper motor                  |
| 🔄 **NEMA 17**           | Provides controlled rotary motion                 |
| ⌨️ **4×4 Keypad**        | User input and parameter selection                |
| 📟 **16×2 I2C LCD**      | Displays operating information                    |
| 🔋 **Power Supply**      | Provides power to the electronic system           |
| 💉 **Syringe Mechanism** | Converts motor rotation into plunger displacement |

---

## 🔌 Pin Configuration

| Device          | Arduino Pin | Function          |
| :-------------- | :---------: | :---------------- |
| ⚙️ A4988 STEP   |    `D10`    | Step pulse        |
| ↔️ A4988 DIR    |    `D11`    | Direction control |
| 🔌 A4988 ENABLE |    `D12`    | Driver enable     |
| 📟 LCD SDA      |     `A4`    | I²C data          |
| 📟 LCD SCL      |     `A5`    | I²C clock         |

---

## ⚙️ Working Principle

The Arduino receives the required operating parameters through the keypad and displays the selected information on the LCD.

The microcontroller generates **STEP** and **DIR** signals for the A4988 driver. The A4988 uses these control signals to drive the NEMA 17 stepper motor.

The motor rotates the mechanical transmission connected to the syringe plunger. By controlling the **number and frequency of step pulses**, the system controls the plunger displacement and movement speed.

### 🔄 Control Flow

```text
       User Input
           │
           ▼
      ┌──────────┐
      │ Arduino  │
      └────┬─────┘
           │
      STEP / DIR
           │
           ▼
      ┌──────────┐
      │  A4988   │
      └────┬─────┘
           │
           ▼
      ┌──────────┐
      │  NEMA 17 │
      └────┬─────┘
           │
           ▼
   Mechanical Drive
           │
           ▼
   Syringe Plunger
           │
           ▼
   💧 Fluid Delivery
```

---

## 🧮 Control Concept

The syringe plunger displacement is related to the number of motor steps.

For a stepper motor:

$$
N_{steps} = \frac{\theta}{\theta_{step}}
$$

where:

* \(N_{steps}\) = number of motor steps
* \(\theta\) = required motor rotation
* \(\theta_{step}\) = step angle

For a lead-screw-based mechanism:

$$
d =
\frac{N_{steps}}{N_{steps/rev}}
\times P
$$

where:

* \(d\) = plunger displacement
* \(N_{steps/rev}\) = motor steps per revolution
* \(P\) = lead-screw pitch

The delivered volume can be related to syringe cross-sectional area:

$$
V = A d
$$

where:

* \(V\) = delivered volume
* \(A\) = internal cross-sectional area of the syringe
* \(d\) = plunger displacement

These relationships provide the basis for converting the desired fluid-delivery operation into motor movement.

---

## 💻 Software

The firmware was developed using the **Arduino IDE**.

### Main software functions

```text
1. System Initialization
        ↓
2. LCD Initialization
        ↓
3. Keypad Input
        ↓
4. Parameter Selection
        ↓
5. Motor Direction Control
        ↓
6. STEP Pulse Generation
        ↓
7. Plunger Movement
        ↓
8. LCD Feedback
```

### Technologies

<p>
<img src="https://img.shields.io/badge/Arduino-Embedded%20C%2FC%2B%2B-00979D?style=flat-square&logo=arduino&logoColor=white"/>
<img src="https://img.shields.io/badge/IDE-Arduino-00979D?style=flat-square&logo=arduino&logoColor=white"/>
<img src="https://img.shields.io/badge/Simulation-Wokwi-purple?style=flat-square"/>
</p>

---

## 🧪 Wokwi Simulation

The control system was tested using **Wokwi** to verify the microcontroller logic and peripheral connections.

### ▶️ Run the Simulation

**[🚀 Open Wokwi Simulation](https://wokwi.com/projects/446316151195038721)**

---

## 🔬 Testing

The system was tested progressively to verify the operation of individual subsystems and the integrated pump.

### Tests Performed

| Test                 | Purpose                           |
| :------------------- | :-------------------------------- |
| ⚙️ Motor Rotation    | Verify stepper motor operation    |
| ↔️ Direction Control | Verify forward/reverse movement   |
| ⌨️ Keypad            | Verify user input                 |
| 📟 LCD               | Verify display operation          |
| 🔌 A4988 Driver      | Verify motor-driver interface     |
| 💉 Plunger Movement  | Verify mechanical actuation       |
| 💧 Fluid Delivery    | Observe controlled fluid movement |
| 🔁 Repeatability     | Check repeated operation          |

> Quantitative accuracy values should only be reported when supported by experimental measurements.

---

## 📸 Prototype

> **Add your actual project photograph here.**

```text
testing/
└── images/
    └── prototype.jpg
```

After adding the image, use:

```markdown
<p align="center">
  <img src="testing/images/prototype.jpg" width="700">
</p>
```

---

## 🌐 Applications

The underlying concept can be adapted for controlled low-volume fluid delivery in:

* 🧪 Laboratory experiments
* 🎓 Educational demonstrations
* ⚗️ Chemical dispensing
* 🔬 Biomedical prototyping
* 🏭 Automated fluid handling

> ⚠️ **Note:** This is an academic/prototype implementation and is **not intended for clinical use**.

---

## 🚀 Future Improvements

Possible extensions include:

* 🔄 Closed-loop flow control
* 📡 Encoder-based feedback
* 🎯 Automatic calibration
* ⚙️ Improved mechanical transmission
* 🔬 Higher-resolution microstepping
* 💧 Flow-rate monitoring
* 💾 SD-card data logging
* 📶 Wireless monitoring
* 🛑 Emergency-stop and fault detection
* 🧱 Improved mechanical enclosure and stability

---

## 🧰 Technologies & Tools

### Hardware

`Arduino` • `A4988` • `NEMA 17` • `4×4 Keypad` • `16×2 I2C LCD`

### Software

`Embedded C/C++` • `Arduino IDE` • `Wokwi`

---

## 📂 Repository Structure

```text
low-cost-modular-syringe-pump/
│
├── 📄 README.md
│
├── 📁 src/
│   └── syringe_pump.ino
│
├── 📁 hardware/
│   ├── circuit_diagram/
│   │   └── syringe_pump_circuit.png
│   └── pin_configuration.md
│
├── 📁 simulation/
│   └── wokwi/
│
├── 📁 documentation/
│   └── project_report.pdf
│
└── 📁 testing/
    ├── test_results.md
    └── images/
```

---

## 👨‍💻 Author

### **Tharun Banoth**

🎓 IIT Mandi
💻 Embedded Systems & Hardware Enthusiast

🔗 **GitHub:** [@tharun479](https://github.com/tharun479)

---

<p align="center">
  ⭐ If you found this project interesting, consider giving the repository a star!
</p>

<p align="center">
  <b>Built with Arduino • Stepper Motor Control • Embedded Systems</b>
</p>
