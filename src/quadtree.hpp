#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "binding.hpp"
#include "quadtree.hpp"
#include "geometry.hpp"

#include "geos/index/quadtree/Quadtree.h"

class Item : public ObjectWrap {
    public:
        Item(std::string k, geos::geom::Geometry *geom);
        ~Item();
        Item();
        static void Initialize(Handle<Object> target);
        static Handle<Value> New(std::string k, geos::geom::Geometry *geom);
        static Handle<Value> New(Item *item);
        static Handle<Value> New(const Arguments& args);
        static Persistent<FunctionTemplate> constructor;
        geos::geom::Geometry *_geom;
        std::string _key;

};

class Quadtree : public ObjectWrap {
    public:
        Quadtree();
        ~Quadtree();
        static void Initialize(Handle<Object> target);
        static Persistent<FunctionTemplate> constructor;
        static Handle<Value> New(const Arguments& args);
        static Handle<Value> Insert(const Arguments& args);
        static Handle<Value> Remove(const Arguments& args);
        static Handle<Value> QueryAll(const Arguments& args);
        static Handle<Value> Query(const Arguments& args);

    protected:

    private:
        geos::index::quadtree::Quadtree *_quadtree;
};
#endif