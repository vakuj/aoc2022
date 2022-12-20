# Task day 16

## Summary

From https://adventofcode.com/2022/day/16

---

### Part 1

The sensors have led you to the origin of the distress signal: yet another handheld device, just like the one the Elves gave you. However, you don't see any Elves around; instead, the device is surrounded by elephants! They must have gotten lost in these tunnels, and one of the elephants apparently figured out how to turn on the distress signal.

The ground rumbles again, much stronger this time. What kind of cave is this, exactly? You scan the cave with your handheld device; it reports mostly igneous rock, some ash, pockets of pressurized gas, magma... this isn't just a cave, it's a volcano!

You need to get the elephants out of here, quickly. Your device estimates that you have 30 minutes before the volcano erupts, so you don't have time to go back out the way you came in.

You scan the cave for other options and discover a network of pipes and pressure-release valves. You aren't sure how such a system got into a volcano, but you don't have time to complain; your device produces a report (your puzzle input) of each valve's flow rate if it were opened (in pressure per minute) and the tunnels you could use to move between the valves.

There's even a valve in the room you and the elephants are currently standing in labeled AA. You estimate it will take you one minute to open a single valve and one minute to follow any tunnel from one valve to another. What is the most pressure you could release?

Work out the steps to release the most pressure in 30 minutes. What is the most pressure you can release?

#### Example

For example, suppose you had the following scan output:
```
Valve AA has flow rate=0; tunnels lead to valves DD, II, BB
Valve BB has flow rate=13; tunnels lead to valves CC, AA
Valve CC has flow rate=2; tunnels lead to valves DD, BB
Valve DD has flow rate=20; tunnels lead to valves CC, AA, EE
Valve EE has flow rate=3; tunnels lead to valves FF, DD
Valve FF has flow rate=0; tunnels lead to valves EE, GG
Valve GG has flow rate=0; tunnels lead to valves FF, HH
Valve HH has flow rate=22; tunnel leads to valve GG
Valve II has flow rate=0; tunnels lead to valves AA, JJ
Valve JJ has flow rate=21; tunnel leads to valve II
```

Making your way through the tunnels like this, you could probably open many or all of the valves by the time 30 minutes have elapsed. However, you need to release as much pressure as possible, so you'll need to be methodical. 

Instead, consider this approach at each minute:

1. Valve(s) `NULL` are open : release `0` pressure  -> `move to valve DD`
1. Valve(s) `NULL` are open : release `0` pressure  -> `open valve DD`
1. Valve(s) `DD` are open : release `20` pressure -> `move to valve CC`
1. Valve(s) `DD` are open : release `20` pressure -> `move to valve BB`
1. Valve(s) `DD` are open : release `20` pressure -> `open valve BB`
1. Valve(s) `BB, DD` are open : release `33` pressure -> `move to valve AA`
1. Valve(s) `BB, DD` are open : release `33` pressure -> `move to valve II`
1. Valve(s) `BB, DD` are open : release `33` pressure -> `move to valve JJ`
1. Valve(s) `BB, DD` are open : release `33` pressure -> `open valve JJ`
1. Valve(s) `BB, DD, JJ` are open : release `54` pressure -> `move to valve II`
1. Valve(s) `BB, DD, JJ` are open : release `54` pressure -> `move to valve AA`
1. Valve(s) `BB, DD, JJ` are open : release `54` pressure -> `move to valve DD`
1. Valve(s) `BB, DD, JJ` are open : release `54` pressure -> `move to valve EE`
1. Valve(s) `BB, DD, JJ` are open : release `54` pressure -> `move to valve FF`
1. Valve(s) `BB, DD, JJ` are open : release `54` pressure -> `move to valve GG`
1. Valve(s) `BB, DD, JJ` are open : release `54` pressure -> `move to valve HH`
1. Valve(s) `BB, DD, JJ` are open : release `54` pressure -> `open valve HH`
1. Valve(s) `BB, DD, HH, JJ` are open : release `76` pressure -> `move to valve GG`
1. Valve(s) `BB, DD, HH, JJ` are open : release `76` pressure -> `move to valve FF`
1. Valve(s) `BB, DD, HH, JJ` are open : release `76` pressure -> `move to valve EE`
1. Valve(s) `BB, DD, HH, JJ` are open : release `76` pressure -> `open valve EE`
1. Valve(s) `BB, DD, EE, HH, JJ` are open : release `79` pressure -> `move to valve DD`
1. Valve(s) `BB, DD, EE, HH, JJ` are open : release `79` pressure -> `move to valve CC`
1. Valve(s) `BB, DD, EE, HH, JJ` are open : release `79` pressure -> `open valve CC`
1. Valve(s) `BB, CC, DD, EE, HH, JJ` are open : release `81` pressure
1. Valve(s) `BB, CC, DD, EE, HH, JJ` are open : release `81` pressure
1. Valve(s) `BB, CC, DD, EE, HH, JJ` are open : release `81` pressure
1. Valve(s) `BB, CC, DD, EE, HH, JJ` are open : release `81` pressure
1. Valve(s) `BB, CC, DD, EE, HH, JJ` are open : release `81` pressure
1. Valve(s) `BB, CC, DD, EE, HH, JJ` are open : release `81` pressure

This approach lets you release the most pressure possible in `30 minutes` with this valve layout, `1651`.

---

### Part 2


#### Example



___


