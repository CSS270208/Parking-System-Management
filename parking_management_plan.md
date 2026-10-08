# Parking Management System — OOP Project Plan

## 1. Project Goal

Build a console-based Parking Management System that demonstrates practical object-oriented programming (OOP), file handling, exception handling, and clean software design.

The system registers vehicles, assigns suitable parking slots across multiple floors, creates parking tickets, calculates parking fees, accepts payments, and preserves information in files.

## 2. Core Features (Must Have)

- [ ] Vehicle registration: plate number, vehicle type, owner name
- [ ] Multiple floors and parking-slot types
- [ ] Automatic assignment of the nearest suitable free slot
- [ ] Ticket generation with ID, slot, vehicle, and entry time
- [ ] Exit by ticket ID or plate number
- [ ] Fee calculation by vehicle type, including a minimum charge and daily cap
- [ ] Availability view by floor and slot type
- [ ] Search parked vehicle by plate number
- [ ] File-based data persistence
- [ ] Custom exception handling

### Supported Vehicle Types

| Vehicle | Suitable slot types |
| ------- | ------------------- |
| Bike    | Bike, Compact       |
| Car     | Compact, Large      |
| Truck   | Large               |
| EV Car  | EV, Large           |

### Supported Slot Types

- Bike
- Compact
- Large
- EV
- Handicapped

> Handicapped slots should be assigned only when the vehicle/owner is eligible. This can be implemented as an optional extension if eligibility data is out of the initial scope.

## 3. Good-to-Have Features

- [ ] Normal, weekend, peak-hour, and monthly-pass pricing
- [ ] Cash, card, and UPI payment methods
- [ ] Advance reservations
- [ ] Admin and attendant roles
- [ ] Daily revenue and parking-usage reports
- [ ] Lost-ticket penalty
- [ ] Waiting list when the lot is full
- [ ] Database persistence as a future upgrade

## 4. OOP Class Design

| Class                                            | Type           | Extends / Implements | Responsibility                                                 |
| ------------------------------------------------ | -------------- | -------------------- | -------------------------------------------------------------- |
| `Vehicle`                                        | Abstract class | —                    | Stores plate number, owner name, and vehicle type              |
| `Bike`, `Car`, `Truck`, `EVCar`                  | Class          | `Vehicle`            | Concrete vehicle types                                         |
| `ParkingSlot`                                    | Abstract class | —                    | Stores slot ID/state and defines `canFit(vehicle)`             |
| `BikeSlot`, `CompactSlot`, `LargeSlot`, `EVSlot` | Class          | `ParkingSlot`        | Enforces slot-specific fit rules                               |
| `ParkingFloor`                                   | Class          | —                    | Holds slots and finds a free suitable slot                     |
| `ParkingLot`                                     | Class          | —                    | Coordinates registration, parking, exit, and search operations |
| `Ticket`                                         | Class          | —                    | Represents an active or completed parking visit                |
| `PricingStrategy`                                | Interface      | —                    | Defines `calculateFee(ticket)`                                 |
| `HourlyPricing`, `PeakPricing`                   | Class          | `PricingStrategy`    | Implements different fee policies                              |
| `Payment`                                        | Interface      | —                    | Defines `pay(amount)`                                          |
| `CashPayment`, `CardPayment`, `UPIPayment`       | Class          | `Payment`            | Implements payment methods                                     |
| `User`                                           | Class          | —                    | Base class for a system user and role                          |
| `Admin`, `Attendant`                             | Class          | `User`               | Enforces role-based permissions                                |
| `FileManager`                                    | Class          | —                    | Saves and loads application records                            |
| `ReportService`                                  | Class          | —                    | Generates daily operational reports                            |

## 5. OOP Concept Mapping

| Concept            | Where it is applied                                                                       |
| ------------------ | ----------------------------------------------------------------------------------------- |
| Encapsulation      | Keep fields private; expose validated getters and methods such as `occupy()` and `free()` |
| Inheritance        | `Vehicle` → `Car`, `Bike`, `Truck`; `ParkingSlot` → slot subclasses; `User` → roles       |
| Polymorphism       | Each slot overrides `canFit()`; pricing and payment implementations are interchangeable   |
| Abstraction        | Use abstract classes for shared models and interfaces for changing business policies      |
| Composition        | A `ParkingFloor` owns slots; a `Ticket` refers to one vehicle and one slot                |
| Exception handling | Use domain-specific exceptions for invalid business actions and storage failures          |

## 6. File Handling

Use a `data/` folder to preserve application state between program runs.

```text
data/
├── vehicles.txt
├── parking_slots.txt
├── active_tickets.txt
├── parking_history.txt
└── reports/
    └── daily_report_YYYY-MM-DD.txt
```

### Suggested File Records

```text
# vehicles.txt
DL01AB1234,CAR,Alice

# active_tickets.txt
T-1001,DL01AB1234,F1-C-12,2026-10-08T09:30:00

# parking_history.txt
T-1001,DL01AB1234,F1-C-12,2026-10-08T09:30:00,2026-10-08T12:15:00,90.00
```

### `FileManager` Responsibilities

- Load parking floors and slots at application startup.
- Load active tickets and mark their slots occupied.
- Save the changed slot state and active ticket after a vehicle parks.
- Move a ticket to parking history after the vehicle exits.
- Create a daily report file when an administrator requests one.
- Convert lower-level I/O failures into a meaningful `FileStorageException`.

For a Java implementation, use `java.nio.file.Files` with `BufferedReader`/`BufferedWriter`, or the corresponding safe file APIs of the selected language. Keep file-writing code inside `FileManager`; do not spread it through menu code or model classes.

## 7. Exception Handling

Create a custom parent exception plus specific business exceptions:

```text
ParkingException
├── LotFullException
├── InvalidTicketException
├── DuplicateVehicleException
├── VehicleNotFoundException
├── UnauthorizedActionException
├── PaymentFailedException
└── FileStorageException
```

| Situation                                  | Exception                     | Example user message                                   |
| ------------------------------------------ | ----------------------------- | ------------------------------------------------------ |
| No suitable free slot exists               | `LotFullException`            | `Parking failed: no suitable slot is available.`       |
| Ticket does not exist or is already closed | `InvalidTicketException`      | `Exit failed: ticket ID is invalid.`                   |
| Same plate is already parked               | `DuplicateVehicleException`   | `This vehicle is already parked.`                      |
| Plate is not found                         | `VehicleNotFoundException`    | `No active parking record was found for this plate.`   |
| Attendant attempts an admin-only action    | `UnauthorizedActionException` | `Access denied: administrator permission is required.` |
| Saving/loading data fails                  | `FileStorageException`        | `Data could not be saved. Please try again.`           |

The console UI should catch these exceptions and display helpful messages. It should not expose raw technical stack traces to the normal user.

## 8. Main System Flow

```text
Park Vehicle
  → validate details
  → check duplicate plate
  → find nearest suitable free slot
  → create ticket
  → occupy slot
  → save active state to files
  → show ticket

Exit Vehicle
  → validate ticket or plate
  → calculate fee
  → collect payment
  → free slot
  → archive completed ticket
  → save changed state and update report data
```

## 9. Design Decisions and Trade-Offs

| Decision                             | Reason                                                                   | Trade-off                                                |
| ------------------------------------ | ------------------------------------------------------------------------ | -------------------------------------------------------- |
| Text/CSV-style files first           | Easy to inspect, demonstrate, and submit                                 | Less robust than a database for simultaneous users       |
| `PricingStrategy` interface          | New fee policies can be added without changing exit logic                | Adds more classes for a small project                    |
| Slot subclasses                      | Clearly demonstrates polymorphism and keeps fit logic near the slot type | More classes than using one conditional-heavy slot class |
| Custom exceptions                    | Separates expected business errors from programming defects              | Requires deliberate handling at application boundaries   |
| `Map<String, Ticket>` keyed by plate | Fast lookup for search and duplicate checks                              | Must keep maps synchronized with files                   |

### Practical Data Structures

- `Map<String, Ticket>` for active tickets by vehicle plate.
- `Map<String, Ticket>` for active tickets by ticket ID.
- `List<ParkingFloor>` for floors in nearest-first order.
- `Map<SlotType, List<ParkingSlot>>` inside each floor when optimizing allocation later.

For a student-scale lot, scanning floors and slots is acceptable. For a much larger lot, maintain a queue or set of free slots per floor/type to avoid repeatedly scanning occupied slots.

## 10. Five-Week Development Plan

| Week | Focus         | Tasks                                                                 | Deliverable                                      |
| ---- | ------------- | --------------------------------------------------------------------- | ------------------------------------------------ |
| 1    | Design        | Write requirements, draw UML, decide menu commands and data formats   | Requirements and UML diagram                     |
| 2    | Core models   | Build vehicle, slot, floor, and ticket classes; unit test fit rules   | Working, tested model classes                    |
| 3    | Parking logic | Implement parking, exit, search, availability, fees, and allocation   | End-to-end core workflow                         |
| 4    | Reliability   | Add custom exceptions, file persistence, load-on-startup, and reports | State survives restart; invalid actions are safe |
| 5    | Polish        | Add payments/roles, menu UI, edge-case tests, README, demo data       | Final submission package                         |

## 11. Test Checklist

- [ ] A duplicate active plate is rejected.
- [ ] A truck is not assigned to a bike or compact slot.
- [ ] The nearest suitable slot is selected.
- [ ] Parking when full produces `LotFullException`.
- [ ] Invalid or already-closed tickets produce `InvalidTicketException`.
- [ ] The minimum fee applies to short stays.
- [ ] The daily cap applies correctly for a stay longer than one day.
- [ ] Searching by plate returns the correct floor and slot.
- [ ] An attendant cannot change parking rates or floor configuration.
- [ ] Active tickets and occupied slots are restored after restarting the application.
- [ ] Missing, malformed, or unreadable files are handled with a clear message.
- [ ] A completed ticket is removed from active tickets and added to parking history.
- [ ] A generated daily report contains total revenue and total completed visits.

## 12. Common Beginner Mistakes and Better Approaches

| Beginner mistake                                       | Better engineering approach                                    |
| ------------------------------------------------------ | -------------------------------------------------------------- |
| Put every feature in one `Main` class                  | Separate UI, business logic, models, storage, and reporting    |
| Use large `if/else` chains for every vehicle/slot rule | Use polymorphism through vehicle and slot subclasses           |
| Write files directly from menu code                    | Centralize file operations in `FileManager`                    |
| Catch every exception with a generic message           | Use custom exceptions with actionable user messages            |
| Store money in floating-point values                   | Prefer integer smallest currency units or `BigDecimal` in Java |
| Delete ticket data on exit                             | Preserve completed visits in a history file for reports        |
| Add advanced features before core parking works        | Finish and test the park/exit flow first                       |

## 13. Suggested Console Menu

```text
1. Park vehicle
2. Exit vehicle
3. Search vehicle by plate
4. View availability
5. Generate daily report
6. Admin: manage rates/floors
7. Save data
0. Exit
```

## 14. Engineering Mindset Takeaway

Build the smallest complete flow first: validate input, allocate a slot, issue a ticket, calculate payment, free the slot, and preserve the result. Then improve extensibility with interfaces and inheritance. Treat file failures and invalid user actions as normal system conditions that deserve clear, controlled handling—not as reasons for the program to crash.
