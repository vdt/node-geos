#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <geos/geom/Geometry.h>
#include "binding.hpp"

#define NODE_GEOS_UNARY_PREDICATE(cppmethod, geosmethod)                                \
    typedef struct {                                                                    \
        Geometry *geom;                                                                 \
        Persistent<Function> cb;                                                        \
        bool result;                                                                    \
    } geosmethod##_baton_t;                                                             \
                                                                                        \
    int Geometry::EIO_##cppmethod(eio_req *req) {                                       \
        geosmethod##_baton_t *closure = static_cast<geosmethod##_baton_t *>(req->data); \
        closure->result = closure->geom->_geom->geosmethod();                           \
        return 0;                                                                       \
    }                                                                                   \
                                                                                        \
    int Geometry::EIO_After##cppmethod(eio_req *req) {                                  \
        geosmethod##_baton_t *closure = static_cast<geosmethod##_baton_t *>(req->data); \
        Local<Value> argv[2] = { Local<Value>::New(Null()), Local<Value>::New(closure->result ? True() : False()) }; \
        TryCatch tryCatch;                                                              \
        closure->cb->Call(Context::GetCurrent()->Global(), 2, argv);                    \
                                                                                        \
        if(tryCatch.HasCaught()) {                                                      \
            FatalException(tryCatch);                                                   \
        }                                                                               \
                                                                                        \
        closure->cb.Dispose();                                                          \
        closure->geom->Unref();                                                         \
        ev_unref(EV_DEFAULT_UC);                                                        \
                                                                                        \
        delete closure;                                                                 \
                                                                                        \
        return 0;                                                                       \
                                                                                        \
    }                                                                                   \
    Handle<Value> Geometry::cppmethod(const Arguments& args)                            \
    {                                                                                   \
        HandleScope scope;                                                              \
        Geometry *geom = ObjectWrap::Unwrap<Geometry>(args.This());                     \
        if (args.Length() == 1) {                                                       \
            geosmethod##_baton_t *closure = new geosmethod##_baton_t();                 \
            closure->geom = geom;                                                       \
            closure->cb = Persistent<Function>::New(Handle<Function>::Cast(args[0]));   \
            eio_custom(EIO_##cppmethod, EIO_PRI_DEFAULT, EIO_After##cppmethod, closure);\
            ev_ref(EV_DEFAULT_UC);                                                      \
            geom->Ref();                                                                \
            return Undefined();                                                         \
        } else {                                                                        \
            if (geom->_geom->geosmethod()) {                                            \
                return True();                                                          \
            } else {                                                                    \
                return False();                                                         \
            }                                                                           \
                                                                                        \
        }                                                                               \
    };                                                                                  \
                                                                                        \

#define NODE_GEOS_BINARY_PREDICATE(cppmethod, geosmethod)                   \
    Handle<Value> Geometry::cppmethod(const Arguments& args)                \
    {                                                                       \
        HandleScope scope;                                                  \
        Geometry *geom = ObjectWrap::Unwrap<Geometry>(args.This());         \
        Geometry *geom2 = ObjectWrap::Unwrap<Geometry>(args[0]->ToObject());\
        if (geom->_geom->geosmethod(geom2->_geom)) {                        \
            return True();                                                  \
        } else {                                                            \
            return False();                                                 \
        }                                                                   \
    };                                                                      \

#define NODE_GEOS_UNARY_TOPOLOGIC_FUNCTION(cppmethod, geosmethod)               \
    Handle<Value> Geometry::cppmethod(const Arguments& args) {                  \
        HandleScope scope;                                                      \
        Geometry *geom = ObjectWrap::Unwrap<Geometry>(args.This());             \
        geos::geom::Geometry* result = geom->_geom->geosmethod();               \
        return scope.Close(Geometry::New(result));                              \
    }                                                                           \

#define NODE_GEOS_BINARY_TOPOLOGIC_FUNCTION(cppmethod, geosmethod)              \
    Handle<Value> Geometry::cppmethod(const Arguments& args) {                  \
        HandleScope scope;                                                      \
        Geometry *geom = ObjectWrap::Unwrap<Geometry>(args.This());             \
        Geometry *geom2 = ObjectWrap::Unwrap<Geometry>(args[0]->ToObject());    \
        geos::geom::Geometry* result = geom->_geom->geosmethod(geom2->_geom);   \
        return scope.Close(Geometry::New(result));                              \
    }                                                                           \

#define NODE_GEOS_DOUBLE_GETTER(cppmethod, geosmethod)                          \
    Handle<Value> Geometry::cppmethod(const Arguments& args) {                  \
        HandleScope scope;                                                      \
        Geometry *geom = ObjectWrap::Unwrap<Geometry>(args.This());             \
        return scope.Close(Number::New(geom->_geom->geosmethod()));             \
    }                                                                           \

#define NODE_GEOS_V8_FUNCTION(cppmethod) \
    static Handle<Value> cppmethod(const Arguments& args); \
    static int EIO_##cppmethod(eio_req *req); \
    static int EIO_After##cppmethod(eio_req *req); \

class Geometry : public ObjectWrap {
 public:
    geos::geom::Geometry* _geom;
    Geometry();
    Geometry(geos::geom::Geometry* geom);
    ~Geometry();
    static void Initialize(Handle<Object> target);
    static Handle<Value> New(geos::geom::Geometry* geometry);

    void _ref() { Ref(); };
    void _unref() { Unref(); };

 protected:
    static Handle<Value> New(const Arguments& args);
    // GEOS unary predicates
    NODE_GEOS_V8_FUNCTION(IsSimple);
    NODE_GEOS_V8_FUNCTION(IsValid);
    NODE_GEOS_V8_FUNCTION(IsEmpty);
    NODE_GEOS_V8_FUNCTION(IsRectangle);

    // GEOS binary predicates
    static Handle<Value> Disjoint(const Arguments& args);
    static Handle<Value> Touches(const Arguments& args);
    static Handle<Value> Intersects(const Arguments& args);
    static Handle<Value> Crosses(const Arguments& args);
    static Handle<Value> Within(const Arguments& args);
    static Handle<Value> Contains(const Arguments& args);
    static Handle<Value> Overlaps(const Arguments& args);
    static Handle<Value> Equals(const Arguments& args);
    static Handle<Value> Covers(const Arguments& args);
    static Handle<Value> CoveredBy(const Arguments& args);

    //static Handle<Value> EqualsExact(const Arguments& args);
    static Handle<Value> IsWithinDistance(const Arguments& args);

    // GEOS topologic function
    static Handle<Value> Intersection(const Arguments& args);
    static Handle<Value> Union(const Arguments& args);
    static Handle<Value> Difference(const Arguments& args);
    static Handle<Value> SymDifference(const Arguments& args);

    static Handle<Value> GetBoundary(const Arguments& args);
    static Handle<Value> GetEnvelope(const Arguments& args);
    static Handle<Value> ConvexHull(const Arguments& args);

    static Handle<Value> Buffer(const Arguments& args);

    static Handle<Value> Distance(const Arguments& args);

    static Handle<Value> GetArea(const Arguments& args);
    static Handle<Value> GetLength(const Arguments& args);

    static Handle<Value> GetSRID(const Arguments& args);
    static Handle<Value> SetSRID(const Arguments& args);

    static Handle<Value> GetGeometryType(const Arguments& args);

 private:
    static Persistent<FunctionTemplate> constructor;
    static Handle<Value> ToString(const Arguments& args);

};
#endif
