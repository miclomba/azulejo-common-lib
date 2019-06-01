import unittest

from JavaApp import AwtJavaApp, CmdJavaApp

class Test_JavaApp(unittest.TestCase):

    def test_run_script_on_jvm(self):
        cmdApp = CmdJavaApp()

        script = 'java.lang.String.format("Hello, %s!", greetee);'
        inputs =  dict(greetee='world')

        try:
            cmdApp.start(script, inputs)
        except ExceptionType:
            self.fail("could not run script on JVM")

    @unittest.skip
    def test_run_awt_on_jvm(self):
        # GUI app
        awtApp = AwtJavaApp()

        script = """
            new java.lang.Runnable() {
                run: function() {
                    with(JavaImporter(java.awt.Frame)) Frame().setVisible(true);
                }
            };"""

        try:
            awtApp.start(script)
        except ExceptionType:
            self.fail("could not run script on JVM")

    def test_javascript_on_jvm(self):
        cmdApp = CmdJavaApp()

        inputs = dict(val=-1.5)
        outputs = {"result": None}
        script = 'var result = java.lang.Math.abs(val);'

        try:
            cmdApp.start(script, inputs, outputs)
        except ExceptionType:
            self.fail("could not run script on JVM")

        self.assert_(outputs["result"] == 1.5)

