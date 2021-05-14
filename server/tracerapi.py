from elasticsearch import Elasticsearch
from elasticsearch import helpers

class ESTracerAPI():
	def __init__(self):
		self.client = Elasticsearch()

	def add_bulk(self, actions):
		helpers.bulk(self.client, actions)

	def add_doc(self, etype, info):
		self.client.index(index=etype, body=info)

	def get_doc(self, etype, id):
		return self.client.get(index=etype, id=id)

	def get_all_docs(self, etype):
		return self.client.search(index=etype, body={"query":{"match_all":{}}})

	def get_all_indices(self):
		return self.client.indices.get_alias('*')
