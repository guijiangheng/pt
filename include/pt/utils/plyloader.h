#ifndef PT_UTILS_PLYLOADER_H
#define PT_UTILS_PLYLOADER_H

#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include <pt/shapes/triangle.h>

namespace pt {

enum class TypeId {
    INT8, UINT8, INT16, UINT16, INT32, UINT32, FLOAT, DOUBLE, UNKNOWN
};

template <typename T> constexpr TypeId typeId()     { return TypeId::UNKNOWN; }
template<> constexpr TypeId typeId<int8_t>()        { return TypeId::INT8;    }
template<> constexpr TypeId typeId<uint8_t>()       { return TypeId::UINT8;   }
template<> constexpr TypeId typeId<int16_t>()       { return TypeId::INT16;   }
template<> constexpr TypeId typeId<uint16_t>()      { return TypeId::UINT16;  }
template<> constexpr TypeId typeId<int32_t>()       { return TypeId::INT32;   }
template<> constexpr TypeId typeId<uint32_t>()      { return TypeId::UINT32;  }
template<> constexpr TypeId typeId<float>()         { return TypeId::FLOAT;   }
template<> constexpr TypeId typeId<double>()        { return TypeId::DOUBLE;  }

template<typename T> std::string typeName()       { return "unknown"; }
template<> std::string typeName<int8_t>()         { return "char";    }
template<> std::string typeName<uint8_t>()        { return "uchar";   }
template<> std::string typeName<int16_t>()        { return "short";   }
template<> std::string typeName<uint16_t>()       { return "ushort";  }
template<> std::string typeName<int32_t>()        { return "int";     }
template<> std::string typeName<uint32_t>()       { return "uint";    }
template<> std::string typeName<float>()          { return "float";   }
template<> std::string typeName<double>()         { return "double";  }

template <typename T>
struct TypeChain {
    static_assert(typeId<T>() != TypeId::UNKNOWN, "PLY Loader: unknown type.");
    static constexpr auto hasChildType = false;
    using type = T;
};

template<> struct TypeChain<int32_t>  { static constexpr auto hasChildType = true;  using type = int16_t;  };
template<> struct TypeChain<int16_t>  { static constexpr auto hasChildType = true;  using type = int8_t;   };
template<> struct TypeChain<int8_t>   { static constexpr auto hasChildType = true;  using type = uint32_t; };
template<> struct TypeChain<uint32_t> { static constexpr auto hasChildType = true;  using type = uint16_t; };
template<> struct TypeChain<uint16_t> { static constexpr auto hasChildType = true;  using type = uint8_t;  };
template<> struct TypeChain<double>   { static constexpr auto hasChildType = true;  using type = float;    };

class Property {
public:
    explicit Property(const std::string& name) noexcept : name(name) { }
    virtual ~Property() = default;
    virtual void parse(std::istringstream& lineStream) = 0;
    virtual void read(std::ifstream& stream) = 0;
    virtual void writeHeader(std::ofstream& stream) const = 0;
    virtual void writeText(std::ofstream& stream, size_t iElement) const = 0;
    virtual void writeBinary(std::ofstream& stream, size_t iElement) const = 0;
    virtual void reserve(size_t count) = 0;
    virtual size_t size() const = 0;
    virtual std::string getPropertyTypeName() const = 0;

public:
    std::string name;
};

template <typename T>
class TypedProperty final : public Property {
    static_assert(typeId<T>() != TypeId::UNKNOWN, "PLY Loader: unknown property type.");

public:
    explicit TypedProperty(const std::string& name) noexcept : Property(name)
    { }

    TypedProperty(const std::string& name, std::vector<T>&& data) noexcept
        : Property(name), data(std::move(data))
    { }

    void parse(std::istringstream& lineStream) override {
        T val;
        lineStream >> val;
        data.push_back(val);
    }

    void read(std::ifstream& stream) override {
        T val;
        stream.read((char*)&val, sizeof(T));
        data.push_back(val);
    }

    void writeHeader(std::ofstream& stream) const override {
        stream << "property " << typeName<T>() << " " << name << std::endl;
    }

    void writeText(std::ofstream& stream, size_t iElement) const override {
        stream.precision(std::numeric_limits<T>::max_digits10);
        stream << data[iElement] << " ";
    }

    void writeBinary(std::ofstream& stream, size_t iElement) const override {
        stream.write((char*)&data[iElement], sizeof(T));
    }

    void reserve(size_t count) override {
        data.reserve(count);
    }

    size_t size() const override {
        return data.size();
    }

    std::string getPropertyTypeName() const override {
        return typeName<T>();
    }

public:
    std::vector<T> data;
};

template <>
void TypedProperty<uint8_t>::writeText(std::ofstream &stream, size_t iElement) const {
    stream << (int)data[iElement] << " ";
}

template <>
void TypedProperty<int8_t>::writeText(std::ofstream &stream, size_t iElement) const {
    stream << (int)data[iElement] << " ";
}

template <>
void TypedProperty<uint8_t>::parse(std::istringstream &lineStream) {
    int val;
    lineStream >> val;
    data.push_back((uint8_t)val);
}

template <>
void TypedProperty<int8_t>::parse(std::istringstream &lineStream) {
    int val;
    lineStream >> val;
    data.push_back((int8_t)val);
}

template <typename T>
class TypedListProperty : public Property {
public:
    static_assert(typeId<T>() != TypeId::UNKNOWN, "PLY Loader: unknown list property type.");

    explicit TypedListProperty(const std::string& name, int listCountBytes = 1) noexcept
        : Property(name), listCountBytes(listCountBytes)
    { }

    TypedListProperty(const std::string& name, std::vector<std::vector<T>>&& data) noexcept
        : Property(name), data(std::move(data))
    { }

    void writeHeader(std::ofstream& stream) const override {
        stream << "property list uchar " << typeName<T>() << " " << name << std::endl;
    }

    void parse(std::istringstream& lineStream) override {
        int count;
        lineStream >> count;
        T val;
        std::vector<T> vec;
        for (auto i = 0; i < count; ++i) {
            lineStream >> val;
            vec.push_back(val);
        }
        data.push_back(std::move(vec));
    }

    void read(std::ifstream& stream) override {
        int count = 0;
        stream.read((char*)&count, listCountBytes);
        T val;
        std::vector<T> vec;
        for (auto i = 0; i < count; ++i) {
            stream.read((char*)&val, sizeof(T));
            vec.push_back(val);
        }
        data.push_back(std::move(vec));
    }

    void writeText(std::ofstream& stream, size_t iElement) const override {
        auto& elemList = data[iElement];
        auto count = (uint8_t)elemList.size();
        if (count != elemList.size())
            throw std::runtime_error("PLY loader: list property has a element with more entries than fit in a uchar.");
        stream << elemList.size();
        stream.precision(std::numeric_limits<T>::max_digits10);
        for (auto e : elemList) stream << " " << e;
        stream << " ";
    }

    void writeBinary(std::ofstream& stream, size_t iElement) const override {
        auto& elemList = data[iElement];
        auto count = (uint8_t)elemList.size();
        if (count != elemList.size())
            throw std::runtime_error("PLY loader: list property has a element with more entries than fit in a uchar.");
        stream.write((char*)&count, sizeof(uint8_t));
        for (auto e : elemList) stream.write((char*)&e, sizeof(T));
    }

    void reserve(size_t count) override {
        data.reserve(count);
    }

    size_t size() const override {
        return data.size();
    }

    std::string getPropertyTypeName() const override {
        return typeName<T>();
    }

public:
    int listCountBytes;
    std::vector<std::vector<T>> data;
};

template <>
void TypedListProperty<uint8_t>::writeText(std::ofstream &stream, size_t iElement) const {
    auto& elemList = data[iElement];
    auto count = (uint8_t)elemList.size();
    if (count != elemList.size())
        throw std::runtime_error("PLY loader: list property has a element with more entries than fit in a uchar.");
    stream << elemList.size();
    for (auto e : elemList) stream << " " << (int)e;
    stream << " ";
}

template <>
void TypedListProperty<int8_t>::writeText(std::ofstream &stream, size_t iElement) const {
    auto& elemList = data[iElement];
    auto count = (uint8_t)elemList.size();
    if (count != elemList.size())
        throw std::runtime_error("PLY loader: list property has a element with more entries than fit in a uchar.");
    stream << elemList.size();
    for (auto e : elemList) stream << " " << (int)e;
    stream << " ";
}

template <>
void TypedListProperty<uint8_t>::parse(std::istringstream &lineStream) {
    int count;
    lineStream >> count;
    int val;
    std::vector<uint8_t> vec;
    for (auto i = 0; i < count; ++i) {
        lineStream >> val;
        vec.push_back((uint8_t)val);
    }
    data.push_back(std::move(vec));
}

template <>
void TypedListProperty<int8_t>::parse(std::istringstream &lineStream) {
    int count;
    lineStream >> count;
    int val;
    std::vector<int8_t> vec;
    for (auto i = 0; i < count; ++i) {
        lineStream >> val;
        vec.push_back((uint8_t)val);
    }
    data.push_back(std::move(vec));
}

Property* createProperty(
        bool isList, const std::string& name,
        const std::string& typeStr, const std::string& countTypeStr) {

    auto listCountBytes = 0;

    if (isList) {
        if (countTypeStr == "uchar" || countTypeStr == "uint8" ||
            countTypeStr == "char" || countTypeStr == "int8")
            listCountBytes = 1;
        else if (countTypeStr == "ushort" || countTypeStr == "uint16" ||
                 countTypeStr == "short" || countTypeStr == "int16")
            listCountBytes = 2;
        else if (countTypeStr == "uint" || countTypeStr == "uint32" ||
                 countTypeStr == "int" || countTypeStr == "int32")
            listCountBytes = 4;
        else
            throw std::runtime_error("PLY loader: unrecognized list count type: " + countTypeStr + ".");
    }

    if (typeStr == "uchar" || typeStr == "uint8") {
        return isList ?
            (Property*) new TypedListProperty<uint8_t>(name, listCountBytes) :
            (Property*) new TypedProperty<uint8_t>(name);
    } else if (typeStr == "ushort" || typeStr == "uint16") {
        return isList ?
           (Property*) new TypedListProperty<uint16_t>(name, listCountBytes) :
           (Property*) new TypedProperty<uint16_t>(name);
    } else if (typeStr == "uint" || typeStr == "uint32") {
        return isList ?
           (Property*) new TypedListProperty<uint32_t>(name, listCountBytes) :
           (Property*) new TypedProperty<uint32_t>(name);
    }

    if (typeStr == "char" || typeStr == "int8") {
        return isList ?
           (Property*) new TypedListProperty<int8_t>(name, listCountBytes) :
           (Property*) new TypedProperty<int8_t>(name);
    } else if (typeStr == "short" || typeStr == "int16") {
        return isList ?
           (Property*) new TypedListProperty<int16_t>(name, listCountBytes) :
           (Property*) new TypedProperty<int16_t>(name);
    } else if (typeStr == "int" || typeStr == "int32") {
        return isList ?
           (Property*) new TypedListProperty<int32_t>(name, listCountBytes) :
           (Property*) new TypedProperty<int32_t>(name);
    }

    if (typeStr == "float" || typeStr == "float32") {
        return isList ?
           (Property*) new TypedListProperty<float>(name, listCountBytes) :
           (Property*) new TypedProperty<float>(name);
    } else if (typeStr == "double" || typeStr == "float64") {
        return isList ?
           (Property*) new TypedListProperty<double>(name, listCountBytes) :
           (Property*) new TypedProperty<double>(name);
    }

    throw std::runtime_error("PLY loader: unrecognized data type: " + typeStr + ".");
}

template <typename D, typename T>
std::vector<D> getDataFromPropertyRecursive(const Property* prop) {
    auto castedProp = dynamic_cast<const TypedProperty<D>*>(prop);
    if (castedProp) {
        std::vector<D> vec;
        vec.reserve(castedProp->size());
        for (auto e : castedProp->data) vec.emplace_back(static_cast<D>(e));
        return vec;
    }
    if constexpr (TypeChain<T>::hasChildType)
        return getDataFromPropertyRecursive<D, typename TypeChain<T>::type>(prop);
    throw std::runtime_error(
        "PLY loader: property " + prop->name + 
        " cannot be casted to requested type " + typeName<D>() +
        ", which has type " + prop->getPropertyTypeName() + "."
    );
}

template <typename D, typename T>
std::vector<std::vector<D>> getDataFromListPropertyRecursive(const Property* prop) {
    auto castedProp = dynamic_cast<const TypedListProperty<D>*>(prop);
    if (castedProp) {
        std::vector<std::vector<D>> vec;
        vec.reserve(castedProp->size());
        for (auto& list : castedProp->data) {
            std::vector<D> newList;
            newList.reserve(list.size());
            for (auto e : list) newList.emplace_back(static_cast<D>(e));
            vec.emplace_back(std::move(newList));
        }
        return vec;
    }
    if constexpr (TypeChain<T>::hasChildType)
        return getDataFromListPropertyRecursive<D, typename TypeChain<T>::type>(prop);
    throw std::runtime_error(
        "PLY loader: property " + prop->name + 
        " cannot be casted to requested type " + typeName<D>() +
        ", which has type " + prop->getPropertyTypeName() + "."
    );
}

class Element {
public:
    Element(const std::string& name, size_t count) noexcept : name(name), count(count)
    { }

    bool hasProperty(const std::string& name) const {
        for (auto& property : properties)
            if (property->name == name) return true;
        return false;
    }

    const Property* getPropertyPtr(const std::string& name) const {
        for (auto& property : properties)
            if (property->name == name) return property.get();
        throw std::out_of_range("PLY loader: element hove no property with name: " + name + ".");
    }

    template <typename T>
    std::vector<T> getProperty(const std::string& name) const {
        static_assert(typeId<T>() != TypeId::UNKNOWN, "PLY Loader: unknown type.");
        auto prop = getPropertyPtr(name);
        if constexpr (std::is_integral_v<T>)
            return getDataFromPropertyRecursive<T, int32_t>(prop);
        return getDataFromPropertyRecursive<T, double>(prop);
    }

    template <typename T>
    std::vector<std::vector<T>> getListProperty(const std::string& name) const {
        static_assert(typeId<T>() != TypeId::UNKNOWN, "PLY Loader: unknown type.");
        auto prop = getPropertyPtr(name);
        if constexpr (std::is_integral_v<T>)
            return getDataFromListPropertyRecursive<T, int32_t>(prop);
        return getDataFromListPropertyRecursive<T, double>(prop);
    }

    void addProperty(Property* property) {
        if (hasProperty(property->name))
            throw std::runtime_error("PLY loader: element already has property with name: " + name + ".");
        properties.emplace_back(property);
    }

    template <typename T>
    void addProperty(const std::string& name, std::vector<T>&& data) {
        if (data.size() != count)
            throw std::runtime_error("PLY loader: new property " + name + " has size which does not match element");
        if (hasProperty(name))
            throw std::runtime_error("PLY loader: element already has property with name: " + name + ".");
        properties.emplace_back(name, new TypedProperty<T>(name, std::move(data)));
    }

    template <typename T>
    void addListProperty(const std::string& name, std::vector<std::vector<T>>&& data) {
        if (data.size() != count)
            throw std::runtime_error("PLY loader: new property " + name + " has size which does not match element");
        if (hasProperty(name))
            throw std::runtime_error("PLY loader: element already has property with name: " + name + ".");
        properties.emplace_back(name, new TypedListProperty<T>(std::move(data)));
    }

    void writeHeader(std::ofstream& stream) const {
        stream << "element " << name << " " << count << std::endl;
        for (auto& property : properties)
            property->writeHeader(stream);
    }

    void writeText(std::ofstream& stream) const {
        for (size_t i = 0; i < count; ++i) {
            for (auto& property : properties)
                property->writeText(stream, i);
            stream << std::endl;
        }
    }

    void writeBinary(std::ofstream& stream) const {
        for (size_t i = 0; i < count; ++i)
            for (auto& property : properties)
                property->writeBinary(stream, i);
    }

    size_t size() const {
        return count;
    }

public:
    std::string name;
    size_t count;
    std::vector<std::unique_ptr<Property>> properties;
};

enum class DataFormat { Text, Binary };

class PLYData {
public:
    PLYData() noexcept = default;

    explicit PLYData(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (file.fail()) throw std::runtime_error("PLY loader: could not open file " + filename + ".");
        parseHeader(file);
        if (inputFormat == DataFormat::Text) parseTextBody(file);
        else parseBinaryBody(file);
    }

    bool hasElement(const std::string& name) const {
        for (auto& element : elements)
            if (element.name == name) return true;
        return false;
    }

    const Element& getElement(const std::string& name) const {
        for (auto& element : elements)
            if (element.name == name) return element;
        throw std::runtime_error("PLY loader: count not find element with name: " + name + ".");;
    }

    void addElement(const std::string& name, size_t count) {
        if (hasElement(name))
            throw std::runtime_error("PLY loader: element " + name + "already exist.");
        elements.emplace_back(name, count);
    }

    std::vector<Vector3> getVertexPositions() const {
        auto& element = getElement("vertex");
        auto xPos = element.getProperty<Float>("x");
        auto yPos = element.getProperty<Float>("y");
        auto zPos = element.getProperty<Float>("z");
        std::vector<Vector3> vertices;
        auto count = element.size();
        vertices.reserve(count);
        for (size_t i = 0; i < count; ++i)
            vertices.emplace_back(xPos[i], yPos[i], zPos[i]);
        return vertices;
    }

    std::vector<int> getVertexIndices() const {
        auto lists = getElement("face").getListProperty<int32_t>("vertex_indices");
        std::vector<int> indices;
        indices.reserve(indices.size() * 6);
        for (auto& list : lists) {
            if (list.size() != 3 && list.size() != 4) continue;
            indices.emplace_back(list[0]);
            indices.emplace_back(list[1]);
            indices.emplace_back(list[2]);
            if (list.size() == 4) {
                indices.emplace_back(list[3]);
                indices.emplace_back(list[0]);
                indices.emplace_back(list[2]);
            }
        }
        indices.shrink_to_fit();
        return indices;
    }

    void parseHeader(std::istream& stream) {
        std::string line;
        while (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            std::string token;
            lineStream >> token;
            if (token == "ply" || token == "PLY" || token == "comment") continue;
            else if (token == "format") parseFormatHeader(lineStream);
            else if (token == "element") parseElementHeader(lineStream);
            else if (token == "property") parsePropertyHeader(lineStream);
            else if (token == "end_header") break;
        }
    }

    void parseFormatHeader(std::istringstream& stream) {
        std::string type, version;
        stream >> type >> version;
        if (version != "1.0") throw std::runtime_error("PLY loader: only version 1.0 is supported.");
        if (type == "ascii") inputFormat = DataFormat::Text;
        else if (type == "binary_little_endian") inputFormat = DataFormat::Binary;
        else if (type == "binary_big_endian") throw std::runtime_error("PLY loader: big endian binary format not supported.");
        else throw std::runtime_error("PLY loader: unknown file format.");
    }

    void parseElementHeader(std::istringstream& stream) {
        std::string name;
        size_t count;
        stream >> name >> count;
        addElement(name, count);
    }

    void parsePropertyHeader(std::istringstream& stream) {
        if (elements.empty()) throw std::runtime_error("PLY loader: encounter property header before any element header.");
        std::string firstToken;
        stream >> firstToken;
        if (firstToken == "list") {
            std::string countType, type, name;
            stream >> countType >> type >> name;
            elements.back().addProperty(createProperty(true, name, type, countType));
        } else {
            std::string type, name;
            type = firstToken;
            stream >> name;
            elements.back().addProperty(createProperty(false, name, type, ""));
        }
    }

    void parseBinaryBody(std::ifstream& stream) {
        for (auto& element : elements)
            for (std::size_t i = 0; i < element.count; ++i)
                for (auto& property : element.properties)
                    property->read(stream);
    }

    void parseTextBody(std::ifstream& stream) {
        for (auto& element : elements)
            for (std::size_t i = 0; i < element.count; ++i) {
                std::string line;
                std::getline(stream, line);
                std::istringstream lineStream(line);
                for (auto& property : element.properties)
                    property->parse(lineStream);
            }
    }

    void write(const std::string& filename, DataFormat format = DataFormat::Text) const {
        std::ofstream file(filename, std::ios::out | std::ios::binary);
        if (file.fail()) throw std::runtime_error("PLY loader: count not open file " + filename + ".");
        writeHeader(file, format);
        for (auto& element : elements) {
            if (format == DataFormat::Text) element.writeText(file);
            else element.writeBinary(file);
        }
    }

    void writeHeader(std::ofstream& stream, DataFormat format) const {
        stream << "ply" << std::endl;
        stream << "format ";
        if (format == DataFormat::Text) stream << "ascii ";
        else stream << "binary_little_endian ";
        stream << "1.0" << std::endl;
        for (auto& element : elements) element.writeHeader(stream);
        stream << "end_header" << std::endl;
    }

public:
    DataFormat inputFormat = DataFormat::Text;
    std::vector<Element> elements;
};

Mesh loadPLYMesh(const std::string& filename) {
    PLYData ply(filename);
    return Mesh(
        ply.getVertexIndices(),
        ply.getVertexPositions(),
        std::vector<Vector3>(),
        std::vector<Vector2f>()
    );
}

}

#endif
