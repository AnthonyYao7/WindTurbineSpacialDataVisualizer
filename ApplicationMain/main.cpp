#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <memory>
#include <thread>
#include <filesystem>
#include <fstream>
#include <unordered_map>

#include <boost/json.hpp>

#include "trees.h"
#include "CSV_File.h"

#include "SimpleWebServer/server_http.hpp"


using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
namespace fs = std::filesystem;

std::string ui_path = "../../build";
std::string data_path = "../../data";

std::unordered_map<std::string, std::string> mime_types = {
        {"css", "text/css"},
        {"html", "text/html"},
        {"js", "application/javascript"},
        {"svg", "image/svg+xml"},
        {"ico", "image/vnd.microsoft.icon"},
        {"json", "application/json"},
        {"txt", "text/plain"},
        {"png", "image/png"},
};

struct Coordinates
{
    double lat;
    double lon;
};


bool is_valid_file(const std::string& p)
{
    std::string concat = ui_path + p;

    return fs::exists(concat) and fs::is_regular_file(concat);
}


std::string get_content(const std::string& p)
{
    if (not is_valid_file(p))
    {
        std::cout << "file not found" << std::endl;
        return "HTTP/1.1 404 NOT FOUND\r\nContent-Length: 0\r\n\r\n";
    }

    std::string concat = ui_path + p;

    // Read the file into a string
    std::ifstream file_stream(concat);
    if (!file_stream)
        exit(1);

    std::string file_contents((std::istreambuf_iterator<char>(file_stream)),
            std::istreambuf_iterator<char>());

    // Close the file
    file_stream.close();

    int last = 0;

    for (int i = p.size() - 1; i >= 0; --i)
    {
        if (p[i] == '.')
        {
            last = i;
            break;
        }
    }

    std::string file_extension = p.substr(last + 1);

    std::string file_mime_type = mime_types[file_extension];

    return "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(file_contents.length()) + "\r\nContent-Type: " + file_mime_type + "\r\n\r\n" + file_contents;
}


/// using long longs and doubles because we live in the L memory paradigm
struct WindmillData
{
    double lon;
    double lat;
    std::string state;
    std::string county;
    long long year;
    long long capacity;
    double hub_height;
    double rotor_diam;
    double swept_area;
    double total_height;
    double proj_cap;
    long long proj_num_turbines;

    WindmillData() = default;
    WindmillData(
        double lon,
        double lat,
        std::string  state,
        std::string  county,
        long long year,
        long long capacity,
        double hub_height,
        double rotor_diam,
        double swept_area,
        double total_height,
        double proj_cap,
        long long proj_num_turbines
    )
        : lon(lon),
          lat(lat),
          state(std::move(state)),
          county(std::move(county)),
          year(year),
          capacity(capacity),
          hub_height(hub_height),
          rotor_diam(rotor_diam),
          swept_area(swept_area),
          total_height(total_height),
          proj_cap(proj_cap),
          proj_num_turbines(proj_num_turbines)
    {}
};


std::size_t vector_min(const std::vector<size_t>& vec)
{
    return *std::min_element(vec.begin(), vec.end());
}


struct WindmillDataset
{
    std::vector<std::string> states;
    std::vector<std::string> counties;
    std::vector<long long> years;
    std::vector<long long> capacities;
    std::vector<double> hub_heights;
    std::vector<double> rotor_diameters;
    std::vector<double> swept_areas;
    std::vector<double> total_heights;
    std::vector<double> proj_capacities;
    std::vector<long long> proj_num_turbines;
    std::vector<double> lats;
    std::vector<double> lons;

    size_t num_rows;

    explicit WindmillDataset(CSV_File csv)
        : states(csv.column_as_string("Site.State"))
        , counties(csv.column_as_string("Site.County"))
        , years(csv.column_as_int("Year"))
        , capacities(csv.column_as_int("Turbine.Capacity"))
        , hub_heights(csv.column_as_double("Turbine.Hub_Height"))
        , rotor_diameters(csv.column_as_double("Turbine.Rotor_Diameter"))
        , swept_areas(csv.column_as_double("Turbine.Swept_Area"))
        , total_heights(csv.column_as_double("Turbine.Total_Height"))
        , proj_capacities(csv.column_as_double("Project.Capacity"))
        , proj_num_turbines(csv.column_as_int("Project.Number_Turbines"))
        , lats(csv.column_as_double("Site.Latitude"))
        , lons(csv.column_as_double("Site.Longitude"))
    {
        num_rows = vector_min({
            states.size(),
            counties.size(),
            years.size(),
            capacities.size(),
            hub_heights.size(),
            rotor_diameters.size(),
            swept_areas.size(),
            total_heights.size(),
            proj_capacities.size(),
            proj_num_turbines.size(),
            lats.size(),
            lons.size()
        });
    }

    struct Iterator
    {
        size_t ind = 0;
        WindmillDataset& dataset;
        explicit Iterator(WindmillDataset& dataset) : dataset(dataset) {}

        Iterator& operator++()
        {
            ++ind;
            return *this;
        }

        Iterator operator++(int a)
        {
            ind++;
            return *this;
        }

        Iterator& operator--()
        {
            --ind;
            return *this;
        }

        Iterator operator--(int a)
        {
            ind--;
            return *this;
        }

        bool operator == (const Iterator& rhs) const
        {
            return this->ind == rhs.ind;
        }

        bool operator != (const Iterator& rhs) const
        {
            return this->ind != rhs.ind;
        }

        WindmillData operator*()
        {
            return {
                dataset.lons[ind],
                dataset.lats[ind],
                dataset.states[ind],
                dataset.counties[ind],
                dataset.years[ind],
                dataset.capacities[ind],
                dataset.hub_heights[ind],
                dataset.rotor_diameters[ind],
                dataset.swept_areas[ind],
                dataset.total_heights[ind],
                dataset.proj_capacities[ind],
                dataset.proj_num_turbines[ind]
            };
        }
    };

    Iterator begin()
    {
        return Iterator(*this);
    }

    Iterator end()
    {
        Iterator it(*this);
        it.ind = this->num_rows;
        return it;
    }
};



int main()
{
    std::ifstream dataset(data_path + "/wind_turbines.csv");

    if (not dataset.is_open())
    {
        std::cerr << "Could not open wind_turbines.csv" << std::endl;
        return 1;
    }

    CSV_File csv(std::move(dataset));
    WindmillDataset w_dataset(csv);

    std::vector<std::pair<std::array<double, 2>, WindmillData>> datapoints;
    datapoints.reserve(w_dataset.num_rows);

    for (auto it = w_dataset.begin(); it != w_dataset.end(); ++it)
    {
        auto d = *it;
        datapoints.emplace_back(std::array<double, 2>({{d.lat, d.lon}}), d);
    }

    std::cout << "Starting tree construction" << std::endl;

    RangeTree<double, WindmillData, 2> rtree(datapoints.begin(), datapoints.end());
    KD_Tree<double, WindmillData, 2> kdtree(datapoints.begin(), datapoints.end());

    std::cout << "made it past tree construction" << std::endl;

    HttpServer server;
    server.config.port = 8080;

    server.resource["^/static/css/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        *response << get_content(p);
    };

    server.resource["^/static/js/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        *response << get_content(p);
    };

    server.resource["^/static/media/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        *response << get_content(p);
    };

    server.resource["^/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        std::cout << "HI" << std::endl;

        *response << get_content(p);
    };

    server.resource["^/data/[^\\/]+$"]["GET"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        std::string p = request->path;

        std::cout << "HI" << std::endl;

        *response << get_content(p);
    };

//    server.resource["^/query"]["POST"] = [] (std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
//        std::string content = request->content.string();
//
//        /*
//         * the query looks like this:
//         * {"p1": {"lat": lat, "lon": lon},
//         *  "p2": {"lat": lat, "lon": lon}}
//         */
//
//        boost::json::value q = boost::json::parse(content);
//
//        boost::json::object q_obj = q.as_object();
//
//        boost::json::value* p1 = q_obj.if_contains("p1");
//
//        Coordinates upper_left_corner{};
//
//        {
//            if (not p1)
//            {
//                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
//                return;
//            }
//
//            boost::json::object coord_pair = p1->as_object();
//            auto lat = coord_pair.if_contains("lat");
//
//            if (not lat)
//            {
//                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
//                return;
//            }
//
//            upper_left_corner.lat = lat->as_double();
//
//            auto lon = coord_pair.if_contains("lon");
//
//            if (not lon)
//            {
//                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
//                return;
//            }
//
//            upper_left_corner.lon = lon->as_double();
//        }
//
//        boost::json::value* p2 = q_obj.if_contains("p2");
//
//        Coordinates bottom_right_corner{};
//
//        {
//            if (not p2)
//            {
//                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
//                return;
//            }
//
//            boost::json::object coord_pair = p1->as_object();
//            auto lat = coord_pair.if_contains("lat");
//
//            if (not lat)
//            {
//                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
//                return;
//            }
//
//            upper_left_corner.lat = lat->as_double();
//
//            auto lon = coord_pair.if_contains("lon");
//
//            if (not lon)
//            {
//                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
//                return;
//            }
//
//            upper_left_corner.lon = lon->as_double();
//        }
//    };

    std::thread server_thread([&server]() {
        server.start();
    });

    server_thread.join();

    return 0;
}
