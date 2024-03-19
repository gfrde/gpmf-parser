
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <algorithm>

#include "../demo/GPMF_mp4reader.h"
#include "../GPMF_common.h"
#include "../GPMF_parser.h"
#include "../GPMF_utils.h"
#include "CMetadata.h"
#include "CExporterXml.h"
#include "CExporterCsv.h"

#define	SHOW_VIDEO_FRAMERATE		1
#define	SHOW_PAYLOAD_TIME			1
#define	SHOW_ALL_PAYLOADS			0
#define SHOW_GPMF_STRUCTURE			0
#define	SHOW_PAYLOAD_INDEX			0
#define	SHOW_SCALED_DATA			1
#define	SHOW_THIS_FOUR_CC			STR2FOURCC("ACCL")
#define SHOW_COMPUTED_SAMPLERATES	1
#define SHOW_INDEX      			1



uint32_t show_all_payloads = SHOW_ALL_PAYLOADS;
uint32_t show_gpmf_structure = SHOW_GPMF_STRUCTURE;
uint32_t show_payload_index = SHOW_PAYLOAD_INDEX;
uint32_t show_scaled_data = SHOW_SCALED_DATA;
uint32_t show_computed_samplerates = SHOW_COMPUTED_SAMPLERATES;
uint32_t show_video_framerate = SHOW_VIDEO_FRAMERATE;
uint32_t show_payload_time = SHOW_PAYLOAD_TIME;
uint32_t show_this_four_cc = 0;
std::string exportFile;



/**
 * copied from demo-folder
 * modified, to fulfill new requirements
 *
 * @param filename
 * @return
 */
GPMF_ERR readMP4File(char* filename)
{
    GPMF_ERR ret = GPMF_OK;
    GPMF_stream metadata_stream = { 0 }, * ms = &metadata_stream;
    double metadatalength;
    uint32_t* payload = NULL;
    uint32_t payloadsize = 0;
    size_t payloadres = 0;
#if 1 // Search for GPMF Track
    size_t mp4handle = OpenMP4Source(filename, MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE, 0);
#else // look for a global GPMF payload in the moov header, within 'udta'
    size_t mp4handle = OpenMP4SourceUDTA(argv[1], 0);  //Search for GPMF payload with MP4's udta
#endif
    if (mp4handle == 0)
    {
        printf("error: %s is an invalid MP4/MOV or it has no GPMF data\n\n", filename);
        return GPMF_ERROR_BAD_STRUCTURE;
    }

    metadatalength = GetDuration(mp4handle);

    if (metadatalength > 0.0)
    {
        uint32_t index, payloads = GetNumberPayloads(mp4handle);
        printf("found %.2fs of metadata, from %d payloads, within %s\n", metadatalength, payloads, filename);

        uint32_t fr_num, fr_dem;
        uint32_t frames = GetVideoFrameRateAndCount(mp4handle, &fr_num, &fr_dem);
        if (show_video_framerate)
        {
            if (frames)
            {
                printf("VIDEO FRAMERATE:\n  %.3f with %d frames\n", (float)fr_num / (float)fr_dem, frames);
            }
        }

        std::vector<IExporter*> exporterList;
        if (!exportFile.empty()) {
            exporterList.push_back(new CExporterXml(exportFile + ".xml"));


            std::vector<std::string> attrToColumn;
            for (const auto &perType: mappingTypePosition2Element) {
                for (const auto &item: perType.second) {
                    std::string c = perType.first + "_" + item;
                    std::transform(c.begin(), c.end(), c.begin(),
                                   [](unsigned char c){ return std::tolower(c); });

                    attrToColumn.push_back(c);
                }
            }
            exporterList.push_back(new CExporterCsv(exportFile + ".csv", attrToColumn, true));
        }

        for (index = 0; index < payloads; index++)
        {
            double in = 0.0, out = 0.0; //times
            payloadsize = GetPayloadSize(mp4handle, index);
            payloadres = GetPayloadResource(mp4handle, payloadres, payloadsize);
            payload = GetPayload(mp4handle, payloadres, index);
            if (payload == nullptr)
                goto cleanup;

            ret = GetPayloadTime(mp4handle, index, &in, &out);
            if (ret != GPMF_OK)
                goto cleanup;

            ret = GPMF_Init(ms, payload, payloadsize);
            if (ret != GPMF_OK)
                goto cleanup;

            if (show_payload_time)
                if (show_gpmf_structure || show_payload_index || show_scaled_data)
                    if (show_all_payloads || index == 0)
                        printf("PAYLOAD TIME:\n  %.3f to %.3f seconds\n", in, out);

            if (show_gpmf_structure)
            {
                if (show_all_payloads || index == SHOW_INDEX)
                {
                    printf("GPMF STRUCTURE:\n");
                    // Output (printf) all the contained GPMF data within this payload
                    ret = GPMF_Validate(ms, GPMF_RECURSE_LEVELS); // optional
                    if (GPMF_OK != ret)
                    {
                        if (GPMF_ERROR_UNKNOWN_TYPE == ret)
                        {
                            printf("Unknown GPMF Type within, ignoring\n");
                            ret = GPMF_OK;
                        }
                        else
                        {
                            printf("Invalid GPMF Structure\n");
                        }
                    }

                    GPMF_ResetState(ms);

                    GPMF_ERR nextret;
                    do
                    {
                        printf("  ");
//                        PrintGPMF(ms);  // printf current GPMF KLV

                        nextret = GPMF_Next(ms, GPMF_RECURSE_LEVELS | GPMF_TOLERANT);

                        while(nextret == GPMF_ERROR_UNKNOWN_TYPE) // or just using GPMF_Next(ms, GPMF_RECURSE_LEVELS|GPMF_TOLERANT) to ignore and skip unknown types
                            nextret = GPMF_Next(ms, GPMF_RECURSE_LEVELS);

                    } while (GPMF_OK == nextret);
                    GPMF_ResetState(ms);
                }
            }

            if (show_payload_index)
            {
                if (show_all_payloads || index == SHOW_INDEX)
                {
                    printf("PAYLOAD INDEX:\n");
                    ret = GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS|GPMF_TOLERANT);
                    while (GPMF_OK == ret)
                    {
                        ret = GPMF_SeekToSamples(ms);
                        if (GPMF_OK == ret) //find the last FOURCC within the stream
                        {
                            uint32_t key = GPMF_Key(ms);
                            GPMF_SampleType type = GPMF_Type(ms);
                            uint32_t elements = GPMF_ElementsInStruct(ms);
                            //uint32_t samples = GPMF_Repeat(ms);
                            uint32_t samples = GPMF_PayloadSampleCount(ms);

                            if (samples)
                            {
                                printf("  STRM of %c%c%c%c ", PRINTF_4CC(key));

                                if (type == GPMF_TYPE_COMPLEX)
                                {
                                    GPMF_stream find_stream;
                                    GPMF_CopyState(ms, &find_stream);

                                    if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TYPE, GPMF_CURRENT_LEVEL|GPMF_TOLERANT))
                                    {
                                        char tmp[64];
                                        char* data = (char*)GPMF_RawData(&find_stream);
                                        uint32_t size = GPMF_RawDataSize(&find_stream);

                                        if (size < sizeof(tmp))
                                        {
                                            memcpy(tmp, data, size);
                                            tmp[size] = 0;
                                            printf("of type %s ", tmp);
                                        }
                                    }

                                }
                                else
                                {
                                    printf("of type %c ", type);
                                }

                                printf("with %d sample%s ", samples, samples > 1 ? "s" : "");

                                if (elements > 1)
                                    printf("-- %d elements per sample", elements);

                                printf("\n");
                            }

                            ret = GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS|GPMF_TOLERANT);
                        }
                        else
                        {
                            if (ret != GPMF_OK) // some payload element was corrupt, skip to the next valid GPMF KLV at the previous level.
                            {
                                ret = GPMF_Next(ms, GPMF_CURRENT_LEVEL); // this will be the next stream if any more are present.
                                if (ret != GPMF_OK)
                                {
                                    break; //skip to the next payload as this one is corrupt
                                }
                            }
                        }
                    }
                    GPMF_ResetState(ms);
                }
            }

            if (show_scaled_data || !exportFile.empty())
            {
                if (show_all_payloads || index == SHOW_INDEX)
                {
                    CMetadata meta("any source", index, in, out);
                    if (!exporterList.empty()) {
                        if (index == SHOW_INDEX) {
                            for (const auto &exporter: exporterList) {
                                exporter->create(meta);
                            }
                        }

                        for (const auto &exporter: exporterList) {
                            exporter->metadataStart(meta);
                        }
                    }

                    printf("SCALED DATA:\n");
                    while (GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("STRM"), GPMF_RECURSE_LEVELS|GPMF_TOLERANT)) //GoPro Hero5/6/7 Accelerometer)
                    {
                        if (GPMF_VALID_FOURCC(show_this_four_cc))
                        {
                            if (GPMF_OK != GPMF_FindNext(ms, show_this_four_cc, GPMF_RECURSE_LEVELS|GPMF_TOLERANT))
                                continue;
                        }
                        else
                        {
                            ret = GPMF_SeekToSamples(ms);
                            if (GPMF_OK != ret)
                                continue;
                        }

                        char* rawdata = (char*)GPMF_RawData(ms);
                        uint32_t key = GPMF_Key(ms);
                        GPMF_SampleType type = GPMF_Type(ms);
                        uint32_t samples = GPMF_Repeat(ms);
                        uint32_t elements = GPMF_ElementsInStruct(ms);

                        if (samples)
                        {
                            uint32_t buffersize = samples * elements * sizeof(double);
                            GPMF_stream find_stream;
                            double* ptr, * tmpbuffer = (double*)malloc(buffersize);

#define MAX_UNITS	64
#define MAX_UNITLEN	8
                            char units[MAX_UNITS][MAX_UNITLEN] = { "" };
                            uint32_t unit_samples = 1;

                            char complextype[MAX_UNITS] = { "" };
                            uint32_t type_samples = 1;

                            if (tmpbuffer)
                            {
                                uint32_t i, j;

                                //Search for any units to display
                                GPMF_CopyState(ms, &find_stream);
                                if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_SI_UNITS, GPMF_CURRENT_LEVEL | GPMF_TOLERANT) ||
                                    GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_UNITS, GPMF_CURRENT_LEVEL | GPMF_TOLERANT))
                                {
                                    char* data = (char*)GPMF_RawData(&find_stream);
                                    uint32_t ssize = GPMF_StructSize(&find_stream);
                                    if (ssize > MAX_UNITLEN - 1) ssize = MAX_UNITLEN - 1;
                                    unit_samples = GPMF_Repeat(&find_stream);

                                    for (i = 0; i < unit_samples && i < MAX_UNITS; i++)
                                    {
                                        memcpy(units[i], data, ssize);
                                        units[i][ssize] = 0;
                                        data += ssize;
                                    }
                                }

                                //Search for TYPE if Complex
                                GPMF_CopyState(ms, &find_stream);
                                type_samples = 0;
                                if (GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TYPE, GPMF_CURRENT_LEVEL | GPMF_TOLERANT))
                                {
                                    char* data = (char*)GPMF_RawData(&find_stream);
                                    uint32_t ssize = GPMF_StructSize(&find_stream);
                                    if (ssize > MAX_UNITLEN - 1) ssize = MAX_UNITLEN - 1;
                                    type_samples = GPMF_Repeat(&find_stream);

                                    for (i = 0; i < type_samples && i < MAX_UNITS; i++)
                                    {
                                        complextype[i] = data[i];
                                    }
                                }

                                //GPMF_FormattedData(ms, tmpbuffer, buffersize, 0, samples); // Output data in LittleEnd, but no scale
                                if (GPMF_OK == GPMF_ScaledData(ms, tmpbuffer, buffersize, 0, samples, GPMF_TYPE_DOUBLE))//Output scaled data as floats
                                {

                                    ptr = tmpbuffer;
                                    int pos = 0;
                                    for (i = 0; i < samples; i++)
                                    {
                                        if (exportFile.empty()) {
                                            printf("  %c%c%c%c ", PRINTF_4CC(key));
                                        }

                                        char fourcc[5];
                                        sprintf(fourcc, "%c%c%c%c", PRINTF_4CC(key));
                                        fourcc[4] = 0;
                                        std::string fourcc_key = fourcc;

                                        CMetadataEntry entry;


                                        for (j = 0; j < elements; j++)
                                        {
                                            if (type == GPMF_TYPE_STRING_ASCII)
                                            {
                                                std::string s;
                                                s += rawdata[pos];
                                                entry.addValue(s, "");

                                                if (exportFile.empty()) {
                                                    printf("%c", rawdata[pos]);
                                                }

                                                pos++;
                                                ptr++;
                                            }
                                            else if (type_samples == 0) //no TYPE structure
                                            {
                                                std::string s;
                                                s += std::to_string(*ptr);
                                                entry.addValue(s, units[j % unit_samples]);

                                                if (exportFile.empty()) {
                                                    printf("%.5f%s, ", *ptr, units[j % unit_samples]);
                                                }
                                                ptr++;
                                            }
                                            else if (complextype[j] != 'F')
                                            {
                                                std::string s;
                                                s += std::to_string(*ptr);
                                                entry.addValue(s, units[j % unit_samples]);

                                                if (exportFile.empty()) {
                                                    printf("%.8f%s, ", *ptr, units[j % unit_samples]);
                                                }
                                                ptr++;
                                                pos += GPMF_SizeofType((GPMF_SampleType)complextype[j]);
                                            }
                                            else if (type_samples && complextype[j] == GPMF_TYPE_FOURCC)
                                            {
                                                std::string s;
                                                s += "'";
                                                s += rawdata[pos];
                                                s += rawdata[pos+1];
                                                s += rawdata[pos+2];
                                                s += rawdata[pos+3];
                                                s += "'";
                                                entry.addValue(s, "");


                                                ptr++;
                                                if (exportFile.empty()) {
                                                    printf("'%c%c%c%c', ", rawdata[pos], rawdata[pos + 1],
                                                           rawdata[pos + 2], rawdata[pos + 3]);
                                                    printf(" (size=%d), ",
                                                           GPMF_SizeofType((GPMF_SampleType) complextype[j]));
                                                }
                                                pos += GPMF_SizeofType((GPMF_SampleType)complextype[j]);
                                            }
                                        }

                                        meta.addEntry(fourcc_key, entry);
//                                        std::cout << "   --> " << entry;
                                        if (exportFile.empty()) {
                                            printf("\n");
                                        }
                                    }
                                }
                                free(tmpbuffer);
                            }
                        }
                    }
                    GPMF_ResetState(ms);

//                    std::cout << "   --> " << meta;
                    printf("\n");

                    for (const auto &exporter: exporterList) {
                        for (auto it = meta.getEntriesPerType().begin();
                             it != meta.getEntriesPerType().end(); it++) {
                            exporter->typeStart(meta, it->first);
                            int idx = 0;
                            for (const auto &item: it->second) {
                                exporter->write(meta, it->first, it->second.size(), idx, item);
                                idx++;
                            }
                            exporter->typeStop(meta, it->first);
                        }

                        exporter->metadataStop(meta);
                    }
                }
            }
        }


        if (show_computed_samplerates)
        {
            mp4callbacks cbobject;
            cbobject.mp4handle = mp4handle;
            cbobject.cbGetNumberPayloads = GetNumberPayloads;
            cbobject.cbGetPayload = GetPayload;
            cbobject.cbGetPayloadSize = GetPayloadSize;
            cbobject.cbGetPayloadResource = GetPayloadResource;
            cbobject.cbGetPayloadTime = GetPayloadTime;
            cbobject.cbFreePayloadResource = FreePayloadResource;
            cbobject.cbGetEditListOffsetRationalTime = GetEditListOffsetRationalTime;

            printf("COMPUTED SAMPLERATES:\n");
            // Find all the available Streams and compute they sample rates
            while (GPMF_OK == GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS | GPMF_TOLERANT))
            {
                if (GPMF_OK == GPMF_SeekToSamples(ms)) //find the last FOURCC within the stream
                {
                    double start, end;
                    uint32_t fourcc = GPMF_Key(ms);

                    double rate = GetGPMFSampleRate(cbobject, fourcc, STR2FOURCC("SHUT"), GPMF_SAMPLE_RATE_PRECISE, &start, &end);// GPMF_SAMPLE_RATE_FAST);
                    printf("  %c%c%c%c sampling rate = %fHz (time %f to %f)\",\n", PRINTF_4CC(fourcc), rate, start, end);
                }
            }
        }

        cleanup:
        if (payloadres) FreePayloadResource(mp4handle, payloadres);
        if (ms) GPMF_Free(ms);
        CloseSource(mp4handle);

        for (const auto &exporter: exporterList) {
            exporter->close();
            delete exporter;
        }
        exporterList.clear();
    }

    if (ret != GPMF_OK)
    {
        if (GPMF_ERROR_UNKNOWN_TYPE == ret)
            printf("Unknown GPMF Type within\n");
        else
            printf("GPMF data has corruption\n");
    }

    return ret;
}


int main(int argc, char* argv[])
{

    // get file return data
    if (argc < 2)
    {
//        printHelp(argv[0]);
        return -1;
    }

    for (int i = 2; i < argc; i++)
    {
        if (argv[i][0] == '-') //feature switches
        {
            switch (argv[i][1])
            {
                case 'a': show_all_payloads ^= 1;				break;
                case 'g': show_gpmf_structure ^= 1;				break;
                case 'i': show_payload_index ^= 1;				break;
                case 's': show_scaled_data ^= 1;				break;
                case 'c': show_computed_samplerates ^= 1;		break;
                case 'v': show_video_framerate ^= 1;			break;
                case 't': show_payload_time ^= 1;				break;
                case 'f': show_this_four_cc = STR2FOURCC((&(argv[i][2])));  break;
                case 'o':
                    exportFile = (&(argv[i][2]));
                    printf("file to export to: %s\n", exportFile.c_str());
                    break;
//                case 'h': printHelp(argv[0]);  break;
//
//                case 'M':  mp4fuzzchanges = atoi(&argv[i][2]);	break;
//                case 'G':  gpmffuzzchanges = atoi(&argv[i][2]); break;
//                case 'F':  fuzzloopcount = atoi(&argv[i][2]);	break;
            }
        }
    }

    GPMF_ERR ret = readMP4File(argv[1]);

    return 0;
}
