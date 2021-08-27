import subprocess

import python_client_tests
import python_async_tests
import python_http_tests

def algo_tests():
    p_unit_test = subprocess.Popen(["./r_exasol_tests", "[algo]"])
    p_unit_test.wait()
    assert p_unit_test.returncode == 0


if __name__ == "__main__":
    for protocol in ["Http", "Https"]:
        python_http_tests.reading_test(protocol)
        python_http_tests.writing_test(protocol)
        python_http_tests.reading_test_big(protocol)
        python_client_tests.con_controller_read_test(protocol)
        python_client_tests.con_controller_echo_test(protocol)
        python_client_tests.httpTest(protocol)
    python_async_tests.reading_async_test()
    python_async_tests.reading_async_test_abort()
    python_async_tests.writing_async_test()
    python_async_tests.writing_async_test_abort()
    python_client_tests.con_controller_read_test_with_error()
    algo_tests()
