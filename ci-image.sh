set -eu

# A version of the image from a prior build of the same ref name is more likely
# to be useful for caching than the latest prior build.
CACHE_FROM=
if docker pull ${BUILDER_IMAGE}:${CI_COMMIT_REF_SLUG}; then
    CACHE_FROM=${BUILDER_IMAGE}:${CI_COMMIT_REF_SLUG}
elif docker pull ${BUILDER_IMAGE}:latest; then
    CACHE_FROM=${BUILDER_IMAGE}:latest
fi

docker build \
    --cache-from $CACHE_FROM \
    -t ${BUILDER_IMAGE}:${CI_COMMIT_REF_SLUG} \
    -t ${BUILDER_IMAGE}:latest \
    --build-arg GFX_ASSETS_FM_JFROG_TOKEN=${ARTIFACTORY_APIKEY} \
    ./docker/ubuntu1804
