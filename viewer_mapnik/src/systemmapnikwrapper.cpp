#include "systemmapnikwrapper.h"
#include "tbvectordata.h"

#include <boost/make_shared.hpp>
#include <dmsystem.h>
#include <dm.h>

#include <systemmapnikfeatureset.h>
#include <rastermapnikfeatureset.h>

using mapnik::datasource;
using mapnik::parameters;


DATASOURCE_PLUGIN(SystemMapnikWrapper)

SystemMapnikWrapper::SystemMapnikWrapper(mapnik::parameters const & params, bool bind,  DM::System * sys )
    : datasource(params),
      sys(sys),
      desc_(*params.get<std::string>("type"), *params.get<std::string>("encoding","utf-8")),
      extent_()
{
    this->init(params);

}

void SystemMapnikWrapper::init(mapnik::parameters const& params)
{

    this->source_type = *params.get<std::string>("type");
    this->view_name = *params.get<std::string>("view_name");
    this->view_type = *params.get<std::string>("view_type");

    // every datasource must have some way of reporting its extent
    // in this case we are not actually reading from any data so for fun
    // let's just create a world extent in Mapnik's default srs:
    // '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' (equivalent to +init=epsg:4326)
    // see http://spatialreference.org/ref/epsg/4326/ for more details

    std::string view_name = *params.get<std::string>("view_name");
    int view_type = atoi( (*params.get<std::string>("view_type")).c_str() );

    this->view = DM::View(view_name,view_type, DM::READ);

    double x1;
    double y1;
    double x2;
    double y2;

    TBVectorData::GetViewExtend(sys, view, x1, y1, x2, y2);

    if (x1==x2 || y1==y2){
        extent_.init(-180,-90,180,90);
        return;
    }

    extent_.init(x1,y1,x2,y2);

}

SystemMapnikWrapper::~SystemMapnikWrapper() { }

// This name must match the plugin filename, eg 'hello.input'
const char * SystemMapnikWrapper::name()
{
    return "dm";
}

mapnik::datasource::datasource_t SystemMapnikWrapper::type() const
{
    if (this->view.getType() == DM::RASTERDATA)  return datasource::Raster;

    return datasource::Vector;
}

mapnik::box2d<double> SystemMapnikWrapper::envelope() const
{
    return extent_;
}

boost::optional<mapnik::datasource::geometry_t> SystemMapnikWrapper::get_geometry_type() const
{
    return mapnik::datasource::Point;
}

mapnik::layer_descriptor SystemMapnikWrapper::get_descriptor() const
{
    return desc_;
}


mapnik::featureset_ptr SystemMapnikWrapper::features(mapnik::query const& q) const
{
    // if the query box intersects our world extent then query for features
    if (extent_.intersects(q.get_bbox()))
    {
        if (this->view.getType() == DM::RASTERDATA) return boost::make_shared<RasterMapnikFeatureset>(q.get_bbox(),desc_.get_encoding(), sys, this->view);
        return boost::make_shared<SystemMapnikFeatureset>(q.get_bbox(),desc_.get_encoding(), sys, this->view);
    }

    // otherwise return an empty featureset pointer
    return mapnik::featureset_ptr();
}

mapnik::featureset_ptr SystemMapnikWrapper::features_at_point(mapnik::coord2d const& pt,  double tol) const
{
    // features_at_point is rarely used - only by custom applications,
    // so for this sample plugin let's do nothing...
    return mapnik::featureset_ptr();
}
