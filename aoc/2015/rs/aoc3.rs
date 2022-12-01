use std::{collections::HashSet, io::{self, Read}};

type Location = (i32, i32);

fn main() {
    let commands = io::stdin().bytes()
        .map(|result| result.expect("cannot read commands"))
        .map(char::from);

    let count = visit_houses(commands);

    println!("{} houses were visited.", count);
}

fn visit_houses(commands: impl Iterator<Item = char>) -> usize {
    let mut house_map = HashSet::new();
    let mut location: Location = (0, 0);

    house_map.insert(location);

    for command in commands {
        location = move_to(location, command);
        house_map.insert(location);
    }

    house_map.len()
}

fn move_to(from: Location, command: char) -> Location {
    let (x, y) = from;

    match command {
        '<' => (x - 1, y),
        '>' => (x + 1, y),
        '^' => (x, y - 1),
        'v' => (x, y + 1),
        '\r' | '\n' => (x, y), // input has a final newline
        _ => panic!("illegal command {}", command)
    }
}

#[cfg(test)]
fn run_test(input: &str, expected: usize) {
    let result = visit_houses(input.chars());
    assert_eq!(result, expected);
}
#[test]
fn test_single() {
    run_test(">", 2);
}
#[test]
fn test_single_with_newline() {
    run_test(">\r\n", 2);
}
#[test]
fn test_square() {
    run_test("^>v<", 4);
}
#[test]
fn test_wiggle() {
    run_test("^v^v^v^v^v", 2);
}