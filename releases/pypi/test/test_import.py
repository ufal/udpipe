#!/usr/bin/python

import unittest

class TestImport(unittest.TestCase):
    def test_import(self):
        import ufal.udpipe

        self.assertTrue(True)

if __name__ == '__main__':
    unittest.main()
