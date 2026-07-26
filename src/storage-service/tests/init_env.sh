# changing to main dir
cd ../

# applying .env
export $(xargs < .env)

# testing forcing my user to be admin
export AWS_PROFILE=$PROFILE

# running app
./storage_service