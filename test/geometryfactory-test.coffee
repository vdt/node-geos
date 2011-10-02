#!/usr/bin/env coffee

vows = require "vows"
assert = require "assert"

WKTReader = (require "../geos").WKTReader
Geometry = (require "../geos").Geometry
GeometryFactory = (require "../geos").GeometryFactory
PrecisionModel = (require "../geos").PrecisionModel

tests = (vows.describe "GeometryFactory").addBatch

  "A GeometryFactory":
    topic: ->
      new GeometryFactory()

    "a new instance should be an instance of GeometryFactory": (factory) ->
      assert.instanceOf factory, GeometryFactory

  "A GeometryFactory with different PrecisionModel":
    topic: ->
      new GeometryFactory(new PrecisionModel())

    "a new instance should be an instance of GeometryFactory": (factory) ->
      assert.instanceOf factory, GeometryFactory

  "A GeometryFactory with different PrecisionModel and new SRID":
    topic: ->
      new GeometryFactory(new PrecisionModel(), 4326)

    "a new instance should be an instance of GeometryFactory": (factory) ->
      assert.instanceOf factory, GeometryFactory

tests.export module
