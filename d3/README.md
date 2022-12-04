# Task day 3

## Summary

From https://adventofcode.com/2022/day/3

---

### Part 1

Each rucksack has `two large compartments`. `All items of a given type` are meant to go into `exactly one of the two compartments`. The Elf that did the packing `failed to follow this rule` for exactly `one item type per rucksack`.

Every item type is identified by a single lowercase or uppercase letter (that is, a and A refer to different types of items).

The list of items for each rucksack is given as characters all on a single line. A given rucksack always has the same number of items in each of its `two compartments`, so the `first half of the characters` represent items in the `first compartment`, while the `second half of the characters` represent items in the `second compartment`.

To help prioritize item rearrangement, every item type can be converted to a priority:

* `a through z` priorities `1 through 26`.
* `A through Z` priorities `27 through 52`.


Find the item type that appears in both compartments of each rucksack. What is the sum of the priorities of those item types?


### Example


For example, suppose you have the following list of contents from six rucksacks:
```
vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw
```
* The `first` rucksack contains the items `vJrwpWtwJgWrhcsFMMfFFhFp`, which means its first compartment contains the items `vJrwpWtwJgWr`, while the second compartment contains the items `hcsFMMfFFhFp`. The only item type that appears in both compartments is lowercase `p`.
* The `secon`d rucksack's compartments contain `jqHRNqRjqzjGDLGL` and `rsFMfFZSrLrFZsSL`. The only item type that appears in both compartments is uppercase `L`.
* The `third` rucksack's compartments contain `PmmdzqPrV` and `vPwwTWBwg`; the only common item type is uppercase `P`.
* The `fourth` share item type `v`.
* The `fifth` share item type `t`.
* The `sixth` share item type `s`.

In the above example, the priority of the item type that appears in both compartments of each rucksack is 
* `16 (p)`,
* `38 (L)`,
* `42 (P)`,
* `22 (v)`,
* `20 (t)`, and 
* `19 (s)`.

The sum of these is `157`.


---

### Part 2

Additionally, nobody wrote down which item type corresponds to each group's badges. The only way to tell which item type is the right one is by `finding the one item type that is common between all three Elves in each group`.

`Every set of three lines` in your list corresponds to `a single group`, but each group can have a different badge item type.

Find the item type that corresponds to the badges of each three-Elf group. What is the sum of the priorities of those item types?

### Example

In the above example, the first group's rucksacks are the first three lines:

```
vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
```
And the second group's rucksacks are the next three lines:
```
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw
```
In the `first group`, the only item type that appears in `all three` rucksacks is lowercase `r`; this must be their badges. In the `second group`, their badge item type must be 'Z'.

Priorities for these items must still be found to organize the sticker attachment efforts: 
* `18 (r)` for the first group, and
* `52 (Z)` for the second group.

The sum of these is `70`.


___


