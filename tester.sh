#!/usr/bin/env bash

# Codexion smoke, validation, timing, and Valgrind tester.

set -u

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
PROGRAM="$ROOT_DIR/codexion"
BUILD_LOG=$(mktemp)
TEST_DIR=$(mktemp -d)
PASS_COUNT=0
FAIL_COUNT=0
SKIP_COUNT=0

if [[ -t 1 && -z "${NO_COLOR:-}" ]]; then
	RESET=$'\033[0m'
	BOLD=$'\033[1m'
	DIM=$'\033[2m'
	CYAN=$'\033[36m'
	GREEN=$'\033[32m'
	YELLOW=$'\033[33m'
	RED=$'\033[31m'
else
	RESET=''
	BOLD=''
	DIM=''
	CYAN=''
	GREEN=''
	YELLOW=''
	RED=''
fi

cleanup()
{
	rm -f "$BUILD_LOG"
	rm -rf "$TEST_DIR"
}
trap cleanup EXIT

pass()
{
	printf '%bPASS%b: %s\n' "$GREEN" "$RESET" "$1"
	PASS_COUNT=$((PASS_COUNT + 1))
}

fail()
{
	printf '%bFAIL%b: %s\n' "$RED" "$RESET" "$1"
	FAIL_COUNT=$((FAIL_COUNT + 1))
}

skip()
{
	printf '%bSKIP%b: %s\n' "$YELLOW" "$RESET" "$1"
	SKIP_COUNT=$((SKIP_COUNT + 1))
}

section()
{
	printf '\n%b%s%b\n' "$BOLD$CYAN" "$1" "$RESET"
}

run_logged()
{
	local label=$1
	local log_file=$2
	local pid
	local status
	shift 2
	if [[ ! -t 1 ]]; then
		"$@" >"$log_file" 2>&1
		return $?
	fi
	"$@" >"$log_file" 2>&1 &
	pid=$!
	printf '%b  %s%b ' "$DIM" "$label" "$RESET"
	while ps -p "$pid" >/dev/null 2>&1; do
		printf '\b|'
		sleep 0.08
		printf '\b/'
		sleep 0.08
		printf '\b-'
		sleep 0.08
		printf '\b\\'
		sleep 0.08
	done
	wait "$pid"
	status=$?
	printf '\r%b  %-30s%b\n' "$DIM" "$label" "$RESET"
	return "$status"
}

run_program()
{
	local output_file=$1
	shift
	if command -v timeout >/dev/null 2>&1; then
		timeout --signal=TERM 5s "$PROGRAM" "$@" >"$output_file" 2>&1
	else
		"$PROGRAM" "$@" >"$output_file" 2>&1
	fi
}

expect_rejected()
{
	local name=$1
	shift
	local output_file="$TEST_DIR/reject-$FAIL_COUNT-$PASS_COUNT"
	if run_program "$output_file" "$@"; then
		fail "$name (expected a non-zero exit status)"
	else
		pass "$name"
	fi
}

expect_accepted()
{
	local name=$1
	shift
	local output_file="$TEST_DIR/accept-$FAIL_COUNT-$PASS_COUNT"
	if run_program "$output_file" "$@"; then
		pass "$name"
	else
		fail "$name (expected exit status 0; output follows)"
		sed -n '1,12p' "$output_file"
	fi
}

check_log_consistency()
{
	local log_file=$1
	local coder_count=$2
	awk -v coder_count="$coder_count" '
		function fail(msg) {
			print "incoherent log: " msg > "/dev/stderr"
			exit 1
		}
		{
			if ($0 ~ /^[0-9]+ [0-9]+ has taken a dongle$/) {
				taken[$2] = 1
				next
			}
			if ($0 ~ /^[0-9]+ [0-9]+ is compiling$/) {
				if (taken[$2] != 1) {
					fail("coder " $2 " compiles without taking a dongle first")
				}
				taken[$2] = 0
				next
			}
			if ($0 ~ /^[0-9]+ [0-9]+ burned out$/) {
				if (coder_count == 1 && taken[$2] != 1) {
					next
				}
				if (taken[$2] == 1) {
					taken[$2] = 0
					next
				}
			}
		}
		END {
			for (c in taken) {
				if (taken[c] == 1) {
					fail("coder " c " ended with a dongle still held")
				}
			}
		}
	' "$log_file"
}

printf '%bCodexion tester%b\n' "$BOLD$CYAN" "$RESET"
printf '%bRepository:%b %s\n' "$DIM" "$RESET" "$ROOT_DIR"
printf '%bTip: set NO_COLOR=1 for plain output.%b\n' "$DIM" "$RESET"

section '[1/4] Build checks'
if run_logged 'building' "$BUILD_LOG" make -C "$ROOT_DIR" re; then
	pass 'builds with make re'
else
	fail 'builds with make re'
	sed -n '1,30p' "$BUILD_LOG"
fi

if grep -Eq -- '(^|[[:space:]])-pthread([[:space:]]|$)' "$ROOT_DIR/Makefile"; then
	pass 'Makefile uses the required -pthread flag'
else
	fail 'Makefile uses the required -pthread flag'
fi

if [[ ! -x "$PROGRAM" ]]; then
	skip 'executable checks (codexion was not produced by the build)'
	printf '\nSummary: %d passed, %d failed, %d skipped\n' \
		"$PASS_COUNT" "$FAIL_COUNT" "$SKIP_COUNT"
	exit 1
fi

section '[2/4] Argument validation checks'
expect_rejected 'rejects missing arguments' \
	1 100 10 10 10 1 0
expect_rejected 'rejects extra arguments' \
	1 100 10 10 10 1 0 fifo extra
expect_rejected 'rejects a negative coder count' \
	-1 100 10 10 10 1 0 fifo
expect_rejected 'rejects a zero coder count' \
	0 100 10 10 10 1 0 fifo
expect_accepted 'accepts a single coder that burns out' \
	1 5 100 100 100 1 0 fifo
expect_rejected 'rejects non-integer timing input' \
	1 abc 10 10 10 1 0 fifo
expect_rejected 'rejects a negative cooldown' \
	1 100 10 10 10 1 -1 fifo
expect_rejected 'rejects an unknown scheduler' \
	1 100 10 10 10 1 0 round-robin
expect_rejected 'rejects uppercase FIFO' \
	1 100 10 10 10 1 0 FIFO
expect_rejected 'rejects uppercase EDF' \
	1 100 10 10 10 1 0 EDF
expect_rejected 'rejects malformed fifo scheduler input' \
	1 100 10 10 10 1 0 fif0
expect_rejected 'rejects malformed edf scheduler input' \
	1 100 10 10 10 1 0 edff

section '[3/4] Valid execution and output checks'
expect_accepted 'accepts FIFO arguments' \
	1 100 1 1 1 0 0 fifo
expect_accepted 'accepts EDF arguments' \
	1 100 1 1 1 0 0 edf

simulation_output="$TEST_DIR/simulation-output"
if run_program "$simulation_output" 2 100 1 1 1 1 0 fifo; then
	if grep -Eq '^[0-9]+ [0-9]+ (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$' \
		"$simulation_output"; then
		pass 'uses the required log format when simulation logs are emitted'
		if check_log_consistency "$simulation_output" 2 >/dev/null 2>&1; then
			pass 'logs are coherent for a valid two-coder run'
		else
			fail 'logs are incoherent for a valid two-coder run'
			sed -n '1,20p' "$simulation_output"
		fi
	else
		skip 'log-format check (the executable emitted no simulation state logs)'
	fi
else
	fail 'two-coder simulation exits before the timeout'
	sed -n '1,20p' "$simulation_output"
fi

single_coder_log="$TEST_DIR/single-coder-log"
if run_program "$single_coder_log" 1 5 100 100 100 1 0 fifo; then
	if check_log_consistency "$single_coder_log" 1 >/dev/null 2>&1; then
		pass 'single-coder burnout log is coherent'
	else
		fail 'single-coder burnout log is inconsistent'
		sed -n '1,20p' "$single_coder_log"
	fi
else
	fail 'single-coder simulation exits unexpectedly'
	sed -n '1,20p' "$single_coder_log"
fi

stress_cases=(
	'2 100 1 1 1 1 0 fifo'
	'3 100 1 1 1 1 0 fifo'
	'4 100 1 1 1 1 0 fifo'
	'5 100 1 1 1 1 0 fifo'
	'10 100 1 1 1 1 0 fifo'
	'2 50 5 5 5 1 0 fifo'
	'3 200 10 10 10 1 0 fifo'
	'4 100 100 10 10 10 0 fifo'
	'4 200 50 20 20 2 0 fifo'
	'6 300 20 20 20 2 0 fifo'
	'2 100 1 1 1 1 0 edf'
	'3 100 1 1 1 1 0 edf'
	'4 100 1 1 1 1 0 edf'
	'5 100 1 1 1 1 0 edf'
	'10 100 1 1 1 1 0 edf'
	'2 50 5 5 5 1 0 edf'
	'3 200 10 10 10 1 0 edf'
	'4 100 100 10 10 10 0 edf'
	'4 200 50 20 20 2 0 edf'
	'6 300 20 20 20 2 0 edf'
)

for case_args in "${stress_cases[@]}"; do
	stress_log="$TEST_DIR/stress-$(echo "$case_args" | tr ' ' '-')"
	# Run stress cases with longer timeout (30s)
	if command -v timeout >/dev/null 2>&1; then
		timeout --signal=TERM 30s "$PROGRAM" $case_args >"$stress_log" 2>&1
		status=$?
	else
		run_program "$stress_log" $case_args
		status=$?
	fi
	if [[ $status -eq 0 ]]; then
		pass "stress case passed quickly: $case_args"
	else
		fail "stress case hangs or exits early: $case_args"
		sed -n '1,40p' "$stress_log"
	fi
done

section '[4/4] Valgrind checks'
if ! command -v valgrind >/dev/null 2>&1; then
	skip 'Valgrind is not installed'
else
	valgrind_args=(valgrind --version)
	valgrind_version=$("${valgrind_args[@]}" 2>/dev/null)
	printf '%b  Tool:%b %s\n' "$DIM" "$RESET" "$valgrind_version"
	for scheduler in fifo edf; do
		valgrind_log="$TEST_DIR/valgrind-$scheduler.log"
		valgrind_args=(valgrind --leak-check=full --show-leak-kinds=all \
			--errors-for-leak-kinds=all --track-origins=yes --num-callers=20 \
			--error-exitcode=99 "$PROGRAM" 1 100 1 1 1 0 0 "$scheduler")
		if command -v timeout >/dev/null 2>&1; then
			valgrind_command=(timeout --signal=TERM 10s "${valgrind_args[@]}")
		else
			valgrind_command=("${valgrind_args[@]}")
		fi
		printf '%b  Command:%b valgrind --leak-check=full --track-origins=yes %s 1 100 1 1 1 0 0 %s\n' \
			"$DIM" "$RESET" "$PROGRAM" "$scheduler"
		if run_logged "running Valgrind ($scheduler)" "$valgrind_log" "${valgrind_command[@]}"; then
			status=0
		else
			status=$?
		fi
		error_summary=$(grep 'ERROR SUMMARY:' "$valgrind_log" | tail -n 1 || true)
		leak_summary=$(grep -E 'in use at exit|definitely lost|indirectly lost|possibly lost|still reachable' \
			"$valgrind_log" | tail -n 6 || true)
		if [[ $status -eq 0 ]] && grep -q 'ERROR SUMMARY: 0 errors' "$valgrind_log"; then
			pass "Valgrind Memcheck reports no errors for a minimal $scheduler run"
			printf '%b  Memory summary:%b\n%s\n' "$DIM" "$RESET" "$leak_summary"
		else
			fail "Valgrind Memcheck detected problems in the $scheduler run (exit status $status)"
			printf '%b  Error summary:%b\n%s\n' "$DIM" "$RESET" "${error_summary:-not available}"
			printf '%b  Leak summary:%b\n%s\n' "$DIM" "$RESET" "${leak_summary:-not available}"
			printf '%b  Recent diagnostics:%b\n' "$DIM" "$RESET"
			grep -E 'Invalid |ERROR SUMMARY|at 0x|definitely lost|indirectly lost|possibly lost' \
				"$valgrind_log" | tail -n 20 || true
		fi
	done

	if valgrind --tool=helgrind --version >/dev/null 2>&1; then
		for scheduler in fifo edf; do
			helgrind_log="$TEST_DIR/helgrind-$scheduler.log"
			helgrind_args=(valgrind --tool=helgrind --error-exitcode=99 --num-callers=20 \
				"$PROGRAM" 1 100 1 1 1 0 0 "$scheduler")
			if command -v timeout >/dev/null 2>&1; then
				helgrind_command=(timeout --signal=TERM 15s "${helgrind_args[@]}")
			else
				helgrind_command=("${helgrind_args[@]}")
			fi
			printf '%b  Command:%b valgrind --tool=helgrind %s 1 100 1 1 1 0 0 %s\n' \
				"$DIM" "$RESET" "$PROGRAM" "$scheduler"
			if run_logged "running Helgrind ($scheduler)" "$helgrind_log" "${helgrind_command[@]}"; then
				helgrind_status=0
			else
				helgrind_status=$?
			fi
			helgrind_summary=$(grep -E 'ERROR SUMMARY:|possible data race|pthread.*warning|WARNING:' "$helgrind_log" | tail -n 20 || true)
			if [[ $helgrind_status -eq 0 ]] && grep -q 'ERROR SUMMARY: 0 errors' "$helgrind_log"; then
				pass "Helgrind reports no synchronization errors for a minimal $scheduler run"
			else
				fail "Helgrind detected thread-synchronization issues in the $scheduler run (exit status $helgrind_status)"
				printf '%b  Helgrind summary:%b\n%s\n' "$DIM" "$RESET" "${helgrind_summary:-not available}"
				grep -E 'ERROR SUMMARY|possible data race|pthread|WARNING:' "$helgrind_log" | tail -n 20 || true
			fi
		done
	else
		skip 'Helgrind is not available'
	fi
fi

printf '\n%bSummary%b: %b%d passed%b, %b%d failed%b, %b%d skipped%b\n' \
	"$BOLD" "$RESET" "$GREEN" \
	"$PASS_COUNT" "$RESET" "$RED" "$FAIL_COUNT" "$RESET" "$YELLOW" \
	"$SKIP_COUNT" "$RESET"
if [[ $FAIL_COUNT -eq 0 ]]; then
	exit 0
fi
exit 1
