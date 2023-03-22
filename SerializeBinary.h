#include <GLTFSDK/Document.h>
#include <GLTFSDK/Deserialize.h>
#include <GLTFSDK/Serialize.h>
#include <GLTFSDK/GLTFResourceWriter.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/IStreamReader.h>
#include <GLTFSDK/RapidJsonUtils.h>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/Constants.h>

#include <functional>
#include <memory>
#include <vector>

namespace Microsoft::glTF::Toolkit
{
    /// <summary>
    /// Serializes a glTF asset as a glTF binary (GLB) file.
    /// </summary>
    /// <param name="Document">The glTF asset manifest to be serialized.</param>
    /// <param name="inputStreamReader">A stream reader that is capable of accessing the resources used in the glTF asset by URI.</param>
    /// <param name="outputStreamFactory">A stream factory that is capable of creating an output stream where the GLB will be saved, and a temporary stream for
    /// use during the serialization process.</param>
    void SerializeBinary(const Document &document, std::shared_ptr<const IStreamReader> inputStreamReader, std::shared_ptr<const IStreamWriter> outputStreamWriter);

    /// <summary>
    /// Serializes a glTF asset as a glTF binary (GLB) file.
    /// </summary>
    /// <param name="Document">The glTF asset manifest to be serialized.</param>
    /// <param name="resourceReader">A resource reader that is capable of accessing the resources used in the document.</param>
    /// <param name="outputStreamFactory">A stream factory that is capable of creating an output stream where the GLB will be saved, and a temporary stream for
    /// use during the serialization process.</param>
    void SerializeBinary(const Document &document, const GLTFResourceReader &resourceReader, std::shared_ptr<const IStreamWriter> outputStreamWriter);
}