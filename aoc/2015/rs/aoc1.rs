use std::io::{self, Read};

type ElevatorState = (i32, Option<usize>);

fn main() {
	let bytes = io::stdin().bytes()
		.map(|result| result.expect("cannot read instructions"));

		let (floor, basement_time) = run_elevator(bytes);

		println!("Ended up at floor {}.", floor);
		match basement_time {
			None => println!("Never entered the basement."),
			Some(t) => println!("Entered basement at step {}.", t)
		};
}

fn run_elevator(data: impl Iterator<Item = u8>) -> ElevatorState {
	let initial_state: ElevatorState = (0, None);

	data.map(char::from)
		.enumerate()
		.fold(initial_state, exec_instruction)
}

fn exec_instruction(
	(current_floor, basement_time): ElevatorState,
	(idx, instruction): (usize, char)
) -> ElevatorState {
	let new_floor = current_floor + match instruction {
		'(' => 1,
		')' => -1,
		_ => panic!("invalid character in input")
	};

	match basement_time {
		None if new_floor < 0 => (new_floor, Some(idx + 1)),
		_ => (new_floor, basement_time)
	}
}

#[cfg(test)]
fn run_test(input: &str, expected: ElevatorState) {
	let result = run_elevator(input.bytes());
	assert_eq!(result, expected);
}
#[test]
fn test_zero_1() { run_test("(())", (0, None)); }
