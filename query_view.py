import gtk

class List(object):
	def __init__(self, count):
		self.count = count

	def __len__(self):
		return self.count

	def get_row(self, row):
		return None

	def get_item(self, row, col):
		print "get_data(%d, %d)" % (row, col)
		return "(%d, %d)" % (row, col)


import sqlite3 as sqlite

class SQLiteQueryList(List):
	def __init__(self, db, tablename, fields='*', where=None, fetch_count=256):
		self.L = []
		
		SQLquery = "SELECT %s FROM %s" % (fields, tablename)
		if where:
			SQLquery += " WHERE " + where

		count = db.execute("SELECT COUNT(*) FROM %s" % tablename).fetchone()[0]
		self.cursor = db.execute(SQLquery)
		self.fetch_count = fetch_count

		List.__init__(self, count)

	def get_row(self, row):
		try:
			return self.L[row]
		except IndexError:
			d = row - len(self.L) + 1
			d = (d + self.fetch_count - 1)/self.fetch_count
			print "fetching %d rows" % (d*self.fetch_count)
			self.L.extend(self.cursor.fetchmany(d*self.fetch_count))

			try:
				return self.L[row]
			except:
				print len(self.L), row
				raise

	def get_item(self, row, col):
		return self.get_row(row)[col]


class GridView(gtk.DrawingArea):
	def __init__(self, columns, data):
		gtk.DrawingArea.__init__(self)

		self.columns = columns
		self.data   = data
		self.h      = 24
		self.top    = 0
		self.height = len(data)*self.h
		self.width  = sum(item[0] for item in columns)

		self.set_size_request(self.width, self.height)

		self.connect("expose_event", self.on_expose)
		
	def on_expose(self, window, event):
		a = event.area
		f = a[1]/self.h
		k = a[3]/self.h
		print a, f, k
		y = f*self.h
		for i in xrange(f, f+k+1):
			x = 0
			for j, (w, renderer) in enumerate(self.columns):
				r = gtk.gdk.Rectangle(x, y, w, self.h)
				background_area = r
				cell_area = r
				expose_area = cell_area
				renderer.set_property("text", self.data.get_item(i, j))
				renderer.render(self.window, self, background_area, cell_area, expose_area, gtk.CELL_RENDERER_PRELIT)
				x += w

			y += self.h


class Header(gtk.Fixed):
	def __init__(self):
		gtk.Fixed.__init__(self)
		self.columns = []
		self.height = 24

	def do_layout(self):
		y = 0
		x = 0
		for label in self.columns:
			w, _ = label.get_size_request()
			self.move(label, x, y)
			x += w
			

	def add_column(self, title, width):
		l = gtk.Button(title)
		l.set_size_request(width, self.height)
		self.columns.append(l)
		self.put(l, 0, 0)

	def set_size(self, index, width):
		self.columns[index].set_size_request(width, self.height)


if __name__ == '__main__':

	H = Header()
	
	c = sqlite.connect('test')
	r = c.execute("SELECT * FROM files")
	columns = []
	for item in r.description:
		renderer = gtk.CellRendererText()
		columns.append(
			(150, renderer)
		)

		H.add_column(item[0], 150)
	
	H.do_layout()
	r.close()
	
#	l = List(1000000)
	l = SQLiteQueryList(c, 'files', fetch_count=64)
	v = GridView(columns, l)

	vbox = gtk.VBox()
	vbox.pack_start(H)
	vbox.pack_start(v)

	sw = gtk.ScrolledWindow()
	sw.set_shadow_type(gtk.SHADOW_ETCHED_IN)
	sw.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
	sw.set_size_request(640,300)
	sw.add_with_viewport(vbox)


	def print_cell_renderer(*args):
		for w, r in columns:
			print r.get_size(v)

	b1 = gtk.Button("")
	b1.connect("clicked", print_cell_renderer)
	
	d = gtk.Dialog()
	#d.vbox.pack_start(H)
	d.vbox.pack_start(sw)
	d.vbox.pack_start(b1)
	d.show_all()

	# run	
	d.run()
	print "rows fetched: %d of %d" % (len(l.L), l.count)

# vim: ts=4 sw=4
