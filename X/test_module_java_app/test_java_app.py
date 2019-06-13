
import javabridge
import os
import unittest

from JavaApp import AwtJavaApp, CmdJavaApp
from ResourcesModule import *

cwd = os.getcwd()

jars = javabridge.JARS
jars.append(cwd + '\\..\\..\\..\\Frontend\\X\\jars\\xui.jar')
javabridge.start_vm()

class Test_JavaApp(unittest.TestCase):

    def test_frontend_backend_integration(self):

        ui = javabridge.JWrapper(javabridge.make_instance("xui/XUserInterface", "()V"))
        val = ui.pi().doubleValue()

        vec = VecDouble()
        vec.append(val)

        self.assert_(len(vec) == 1)
        self.assert_(vec[0] == val)

    def test_frontend_jwrapper(self):

        expectedValue = 3.14
        ui = javabridge.JWrapper(javabridge.make_instance("xui/XUserInterface", "()V"))
        val = ui.pi().doubleValue()
        self.assert_(val == expectedValue)

    def test_class_wrapper(self):

        class Integer:
            new_fn = javabridge.make_new("java/lang/Integer", "(I)V")
            def __init__(self,i):
                self.new_fn(i)

            intValue = javabridge.make_method("intValue", "()I", "Retrieve the integer value")

        expectedValue = 435
        i = Integer(expectedValue)
        self.assert_(i.intValue() == expectedValue)

    def test_class_jwrapper(self):

        expectedValue = 435
        ii = javabridge.JWrapper(javabridge.make_instance("java/lang/Integer", "(I)V", expectedValue))
        self.assert_(ii.intValue() == expectedValue)

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

