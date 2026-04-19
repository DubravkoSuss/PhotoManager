#include "test_search_APP.h"

void SearchTests::matchesByHashtag() {
    auto photo = std::make_shared<Photo>();
    photo->setHashtags({"summer", "travel"});

    PhotoSearchCriteria criteria;
    criteria.setHashtags({"travel"});

    QVERIFY(criteria.matches(photo));
}

void SearchTests::rejectsBySize() {
    auto photo = std::make_shared<Photo>();
    photo->setFileSize(100);

    PhotoSearchCriteria criteria;
    criteria.setMinSize(200);

    QVERIFY(!criteria.matches(photo));
}
