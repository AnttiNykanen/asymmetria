#!/usr/bin/env python
#
# Test server for developing the HTML content
#

import cherrypy
import os

class Root(object):
        status = 'on'
        dimValue = 85
        
	@cherrypy.expose
        @cherrypy.tools.json_out()
	def getDimmer(self):
		return {'status': self.status, 'dimValue': self.dimValue}

        @cherrypy.expose
        def setDimmer(self, dimValue):
                self.dimValue = dimValue

        @cherrypy.expose
        def setStatus(self, status):
                self.status = status
        

if __name__ == '__main__':
	cherrypy.config.update({
		'tools.staticdir.on': True,
		'tools.staticdir.dir': os.path.join(os.getcwd(), 'html'),
                'tools.caching.on': False
	})
	cherrypy.quickstart(Root(), '/')
