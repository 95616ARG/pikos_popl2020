import benchexec.util as util
import benchexec.tools.template
import benchexec.result as result

class Tool(benchexec.tools.template.BaseTool):
    """
    Required: executable, name, determine_result
    version
    cmdline, working_directory, environment
    get_value_from_output => add <column> tags with custom values.
    """
    def executable(self):
        return util.find_executable('pikos')

    def cmdline(self, executable, options, tasks, propertyfile=None, rlimits={}):
        return [executable] + options + tasks

    def name(self):
        return 'IKOS'

    def determine_result(self, returncode, returnsignal, output, isTimeout):
        """
        If the tool gave a result, this method needs to return one of the
        benchexec.result.RESULT_* strings.
        @param returncode: the exit code of the program, 0 if the program was killed
        @param returnsignal: the signal that killed the program, 0 if program exited itself
        @param output: a list of strings of output lines of the tool (both stdout and stderr)
        @param isTimeout: whether the result is a timeout
        (useful to distinguish between program killed because of error and timeout)
        @return a non-empty string, usually one of the benchexec.result.RESULT_* constants
        https://github.com/sosy-lab/benchexec/blob/master/benchexec/result.py
        """
        status = "OK"
        if returncode != 0:
            status = "ERROR ({0})".format(returncode)

        return status
