#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <map>

#include <rv/Laserscan.h>
#include <rv/IndexedSegment.h>
#include <rv/ParameterList.h>

/**
 * Some useful utility methods for reading/writing of data and parsing.
 */

/** \brief Parsing of directory contents with laser scan files.
 *
 *  \author behley
 */
class DirectoryUtil
{
  public:
    /** \brief parses the directory and generates filenames for scans, segments, labels.
     *
     *  Extracts from the given directory all laser scan filenames and builds
     *  filenames for scans, segments, and annotation files.
     *
     *  \param directoryName    directory name with laser scan files.
     */
    DirectoryUtil(const std::string& directoryName);

    /** \brief number of laser scans in directory. **/
    uint32_t count() const;

    /** \brief next file available?
     *  \return true, if next file available, false otherwise.
     */
    bool hasNextFile() const;

    /** \brief iterate to next laser range scan file. **/
    void next();

    /** \brief retrieve next laserscan filename from directory. **/
    std::string getLaserscanFilename() const;
    /** \brief retrieve next annotation filename from directory. **/
    std::string getAnnotationFilename() const;
    /** \brief retrieve next segment filename from directory. **/
    std::string getSegmentFilename() const;

    /** \brief build next laserscan filename with given directory. **/
    std::string getLaserscanFilename(const std::string& dirname) const;

    /** \brief build next segment filename with given directory. **/
    std::string getSegmentFilename(const std::string& dirname) const;

    /** \brief build next annotation filename with given directory. **/
    std::string getAnnotationFilename(const std::string& dirname) const;

    // void getLaserscanFilenames(const std::vector<std::string>& filenames) const;
    // void getSegmentFilenames(const std::vector<std::string>& filenames) const;
    // void getAnnotationFilenames(const std::vector<std::string>& filenames) const;

  protected:
    std::string dirName_;
    int32_t currentIndex_;
    std::vector<std::string> scannames_;
};

/** \brief get all complete filenames in given directory. **/
std::vector<std::string> getDirectoryListing(const std::string& dirname);
std::vector<std::string> getDirectoryListing(const std::string& dirname, const std::string& extension);

/** \brief remove extensions from filename. **/
std::string stripExtension(const std::string& filename, uint32_t level = 1);

/** utility methods for reading and writing data **/

/** \brief read binary laser range scan from given filename. **/
void readLaserscan(const std::string& filename, rv::Laserscan& scan);

/** \brief read binary segments from given filename. **/
void readSegments(const std::string& filename, std::vector<rv::IndexedSegment>& segments);
/** \brief write the given segments to the file with filename. **/
void writeSegments(const std::string& filename, const std::vector<rv::IndexedSegment>& segments);

/** \brief read KITTI annotation from given filename. **/
void readAnnotations(const std::string& filename, std::vector<std::string>& labels);
/** \brief write KITTI annotation to given filename. **/
void writeAnnotations(const std::string& filename, const std::vector<std::string>& labels,
    const std::vector<float>& prob);

/** \brief read vocabulary from given filename. **/
void readVocabulary(const std::string& filename, std::vector<std::vector<float> >& vocabulary);
/** \brief write vocabulary from given filename. **/
void writeVocabulary(const std::string& filename, const std::vector<std::vector<float> >& vocabulary);

/** \brief parse mapping from label strings to label ids. **/
void parseMapping(const rv::ParameterList& params, std::map<std::string, uint16_t>& label2id,
    std::map<uint16_t, std::string>& id2label);

/** \brief simple progress printing as feedback. **/
void printProgress(uint32_t scan, uint32_t totalScans);

/** \brief compute overlap between two indexed segments according to Intersection-over-Union aka PASCAL VOC criterion
 *
 *  Both segments are from the same scan and therefore the overlap is computed as
 *
 *    | Intersection of both index sets |/| Union of both index sets |
 *
 *  \param first first segment
 *  \param second second segment
 *
 *  \return overlap value \in [0,1]
 */
float overlap(const rv::IndexedSegment& first, const rv::IndexedSegment& second);

#endif
